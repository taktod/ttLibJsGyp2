/*
 * mmAudioLoopbackUtil.c
 * 
 * Created on: 2017/05/03
 *     Author: taktod
 */

#ifdef __ENABLE_WIN32__

#include "mmAudioLoopbackUtil.h"
#include <ttLibC/allocator.h>
#include <ttLibC/log.h>
#include <ttLibC/frame/audio/pcmS16.h>
#include <ttLibC/util/stlListUtil.h>
#include <ttLibC/container/mkv.h>
#include <string.h>

#include <locale.h>
#include <stdio.h>
#include <windows.h>
#include <mmsystem.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <avrt.h>
#include <functiondiscoverykeys_devpkey.h>
#include "clean.h"

#pragma comment(lib, "winmm")
#pragma comment(lib, "Avrt")

class AudioClientStopOnExit {
public:
    AudioClientStopOnExit(IAudioClient *p) : m_p(p) {}
    ~AudioClientStopOnExit() {
        HRESULT hr = m_p->Stop();
        if (FAILED(hr)) {
          puts("AudioClientStop失敗");
        }
    }

private:
    IAudioClient *m_p;
};

class AvRevertMmThreadCharacteristicsOnExit {
public:
    AvRevertMmThreadCharacteristicsOnExit(HANDLE hTask) : m_hTask(hTask) {}
    ~AvRevertMmThreadCharacteristicsOnExit() {
        if (!AvRevertMmThreadCharacteristics(m_hTask)) {
          puts("Threadの優先度解除できなかった。");
        }
    }
private:
    HANDLE m_hTask;
};

class PropVariantClearOnExit {
public:
  PropVariantClearOnExit(PROPVARIANT *p) : m_p(p) {}
  ~PropVariantClearOnExit() {
    HRESULT hr = PropVariantClear(m_p);
    if (FAILED(hr)) {
      puts("PropVariantClear filed.");
    }
  }
private:
  PROPVARIANT *m_p;
};

class CoTaskMemFreeOnExit {
public:
    CoTaskMemFreeOnExit(PVOID p) : m_p(p) {}
    ~CoTaskMemFreeOnExit() {
        CoTaskMemFree(m_p);
    }

private:
    PVOID m_p;
};

class CancelWaitableTimerOnExit {
public:
  CancelWaitableTimerOnExit(HANDLE h) : m_h(h) {}
  ~CancelWaitableTimerOnExit() {
    if(!CancelWaitableTimer(m_h)) {
      puts("fail to cancel waitable timer");
    }
  }
private:
  HANDLE m_h;
};

// ringBufferの中身
typedef struct ttLibC_Util_MmAudioLoopback_Buffer {
  uint8_t  pcm[655360]; // データの保持buffer
  uint32_t length; // 記録済みデータのbufferサイズ
} ttLibC_Util_MmAudioLoopback_Buffer;

typedef ttLibC_Util_MmAudioLoopback_Buffer ttLibC_Buffer;

// ringBufferを扱うクラス。勝手に解放されるようにしとく
class RingBuffer {
public:
  RingBuffer() {
    refPointer_ = -2; // refPointerは2回目以降のring移動で有効にする。
    addPointer_ = -1; // addPointerは1回目のring移動で有効にする。
    is_working_ = true;
    prepareNextBuffer(); // ring移動を実行する。
  }
  ~RingBuffer() {
    // 内部で保持しているデータをすべて解放しなければならない。
    is_working_ = false;
    for(int i = 0;i < 16;++ i) {
      releaseBuffer(buffer[i]);
      buffer[i] = NULL;
    }
  }
  /**
   * 次に利用するbufferを準備する
   */
  void prepareNextBuffer() {
    if(!is_working_) {
      return;
    }
    // bufferをallocしておく。
    ttLibC_Buffer *newBuffer = (ttLibC_Buffer *)ttLibC_malloc(sizeof(ttLibC_Buffer));
    newBuffer->length = 0;
    // ringに登録
    int addPos = (addPointer_ + 1) & 0x0F;
    buffer[addPos] = newBuffer;

    // 処理ポインタを進める
    refPointer_ = addPointer_;
    addPointer_ = addPos;
  }
  /**
   * capture済みbufferを取得する
   * @return ttLibC_Bufferオブジェクト
   */
  ttLibC_Buffer *getRefBuffer() {
    if(!is_working_) {
      return NULL;
    }
    // refPointerが負の数の場合はまだアクセスできるデータがない
    if(refPointer_ < 0) {
      // 生成データがまだない。
      return NULL;
    }
    ttLibC_Buffer *res = buffer[refPointer_];
    buffer[refPointer_] = NULL;
    return res;
  }
  /**
   * capture用のbufferを取得する。
   * ここにthreadでcaptureしたデータを書き込んでいく
   * @return ttLibC_Bufferオブジェクト
   */
  ttLibC_Buffer *getAddBuffer() {
    if(!is_working_) {
      return NULL;
    }
    return buffer[addPointer_];
  }
  /**
   * 取り出したbufferを解放する。
   * そとで解放してもいいけど、このクラスでmallocしてるので、ここでfreeすべきと思う。
   * @param buffer ttLibC_Bufferのオブジェクト
   */
  void releaseBuffer(ttLibC_Buffer *buffer) {
    if(buffer == NULL) {
      return;
    }
    ttLibC_free(buffer);
  }
private:
  ttLibC_Buffer *buffer[16]; // とりあえず16で十分な気がする
  int refPointer_;
  int addPointer_;
  bool is_working_;
};

/**
 * この処理が利用するobject
 */
typedef struct ttLibC_Util_MmAudioLoopback_ {
  ttLibC_MmAudioLoopback inherit_super;
  char locale[256];
  char deviceName[256];
  bool is_running;
  uint64_t totalSampleNum;
  RingBuffer *ringBuffer;
  HANDLE hThread; // 別スレッドを動作させるhandle
  uint32_t channel_num;
  uint32_t sample_rate;
} ttLibC_Util_MmAudioLoopback_;

typedef ttLibC_Util_MmAudioLoopback_ ttLibC_MmAudioLoopback_;

/**
 * 処理実施時のthread動作
 */
static DWORD WINAPI MmAudioLoopback_threadCallback(LPVOID pContext) {
  // コンテキストとして、loopbackオブジェクトを共有しておきます
  ttLibC_MmAudioLoopback_ *loopback = (ttLibC_MmAudioLoopback_ *)pContext;
  if(loopback == NULL) {
    return 0;
  }
  // このthreadの動作localeを設定
  if(strcmp(loopback->locale, "") != 0) { // 未設定なら無視
    setlocale(LC_ALL, loopback->locale);
  }
  // COM初期化
  HRESULT hr = S_OK;
  hr = CoInitialize(NULL);
  if(FAILED(hr)) {
    puts("CoInitialize失敗");
    return 0;
  }
  CoUninitializeOnExit cuoe;

  // deviceEnumeratorを取得する。
  IMMDeviceEnumerator *pMMDeviceEnumerator = NULL;
  hr = CoCreateInstance(
    __uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
    __uuidof(IMMDeviceEnumerator),
    (void **)&pMMDeviceEnumerator);
  if(FAILED(hr)) {
    puts("device enumが取得できなかった。");
    return 0;
  }
  ReleaseOnExit roeDeviceEnumerator(pMMDeviceEnumerator);

  // deviceを取得する、とりあえず選択するのが面倒なので、デフォルトのendpointを取得して利用する。
  IMMDevice *pMMDevice = NULL;
  hr = pMMDeviceEnumerator->GetDefaultAudioEndpoint(
      eRender,
      eConsole,
      &pMMDevice);
  if(FAILED(hr)) {
    puts("deviceEndpoint取得失敗");
    return 0;
  }
  ReleaseOnExit roeDevice(pMMDevice);

  // audioClientを作る
  IAudioClient *pAudioClient = NULL;
  hr = pMMDevice->Activate(
      __uuidof(IAudioClient),
      CLSCTX_ALL, NULL,
      (void **)&pAudioClient);
  if(FAILED(hr)) {
    puts("audioClient取得失敗");
    return 0;
  }
  ReleaseOnExit roeAudioClient(pAudioClient);

  // threadの優先度をAudioのレベルにする
  DWORD nTaskIndex = 0;
  HANDLE hTask = AvSetMmThreadCharacteristics("Audio", &nTaskIndex);
  if(hTask == NULL) {
    puts("thread優先度設定失敗");
    return 0;
  }
  AvRevertMmThreadCharacteristicsOnExit armtcoe(hTask);

  // 該当audioデバイスの動作周期を取得する。
  REFERENCE_TIME hnsDefaultDevicePeriod;
  hr = pAudioClient->GetDevicePeriod(&hnsDefaultDevicePeriod, NULL);
  if(FAILED(hr)) {
    puts("device period取得失敗");
    return 0;
  }

  // 該当audioDeviceの出力フォーマットを取得
  WAVEFORMATEX *pwfx = NULL;
  hr = pAudioClient->GetMixFormat(&pwfx);
  if(FAILED(hr)) {
    puts("getMixFormat失敗");
    return 0;
  }
  CoTaskMemFreeOnExit ctmfoe(pwfx);

  // pcmS16で取得したいので、出力フォーマット情報を更新
  switch (pwfx->wFormatTag) {
  case WAVE_FORMAT_IEEE_FLOAT:
    pwfx->wFormatTag = WAVE_FORMAT_PCM;
    pwfx->wBitsPerSample = 16;
    pwfx->nBlockAlign = pwfx->nChannels * pwfx->wBitsPerSample / 8;
    pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;
    break;
  case WAVE_FORMAT_EXTENSIBLE:
      {
        PWAVEFORMATEXTENSIBLE pEx = reinterpret_cast<PWAVEFORMATEXTENSIBLE>(pwfx);
        if (IsEqualGUID(KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, pEx->SubFormat)) {
            pEx->SubFormat = KSDATAFORMAT_SUBTYPE_PCM;
            pEx->Samples.wValidBitsPerSample = 16;
            pwfx->wBitsPerSample = 16;
            pwfx->nBlockAlign = pwfx->nChannels * pwfx->wBitsPerSample / 8;
            pwfx->nAvgBytesPerSec = pwfx->nBlockAlign * pwfx->nSamplesPerSec;
        } else {
          return 0;
        }
      }
      break;
  default:
    return 0;
  }

  loopback->channel_num = pwfx->nChannels;
  loopback->sample_rate = pwfx->nSamplesPerSec;

  // 一定周期で動作するtimerを作成
  HANDLE hWakeUp = CreateWaitableTimer(NULL, FALSE, NULL);
  if(hWakeUp == NULL) {
    puts("createWatableTimer失敗した。");
    return 0;
  }
  CloseHandleOnExit choeWakeUp(hWakeUp);

  // ringBufferオブジェクト作成
  RingBuffer ringBuffer;
  // loopbackに登録しとく(向こうでも参照したいため)
  loopback->ringBuffer = &ringBuffer;

  // audioClient初期化 取得pcmの設定もいれる
  hr = pAudioClient->Initialize(
      AUDCLNT_SHAREMODE_SHARED,
      AUDCLNT_STREAMFLAGS_LOOPBACK,
      0,
      0,
      pwfx,
      0);
  if(FAILED(hr)) {
    puts("AudioClient initialize失敗");
    return 0;
  }

  // audioCaptureClientを生成
  IAudioCaptureClient *pAudioCaptureClient = NULL;
  hr = pAudioClient->GetService(
    __uuidof(IAudioCaptureClient),
    (void **)&pAudioCaptureClient);
  if(FAILED(hr)) {
    puts("audioCaptureClient取得失敗");
    return 0;
  }
  ReleaseOnExit roeAudioCaptureClient(pAudioCaptureClient);

  // timerの動作ピリオドを設定して起動
  LARGE_INTEGER liFirstFire;
  liFirstFire.QuadPart = -hnsDefaultDevicePeriod / 2;
  LONG lTimerBetweenFires = (LONG)hnsDefaultDevicePeriod / 2 / (10 * 1000);
  BOOL bOK = SetWaitableTimer(
    hWakeUp,
    &liFirstFire,
    lTimerBetweenFires,
    NULL,
    NULL,
    FALSE
  );
  if(!bOK) {
    puts("timerセット失敗");
    return 0;
  }
  CancelWaitableTimerOnExit cancelWakeUp(hWakeUp);

  // 音声キャプチャ開始
  hr = pAudioClient->Start();
  if(FAILED(hr)) {
    puts("AudioClientStart失敗");
    return 0;
  }
  AudioClientStopOnExit stopAudioClient(pAudioClient);

  // loopbackを動作状況に設定する
  loopback->is_running = true; // 開始状況にする。

  while(true) {
    bool local_is_running = loopback->is_running;
    if(!local_is_running) {
      puts("running中じゃなくなった。");
      break;
    }
    // 次のpacketのサイズを調べて、ある場合は処理するというやり方
    uint32_t nNextPacketSize;
    for(hr = pAudioCaptureClient->GetNextPacketSize(&nNextPacketSize);
        SUCCEEDED(hr) && nNextPacketSize > 0;
        hr = pAudioCaptureClient->GetNextPacketSize(&nNextPacketSize)) {
      // byte配列に結果をもらう
      BYTE *pData;
      uint32_t nNumFramesToRead;
      DWORD dwFlags;
      hr = pAudioCaptureClient->GetBuffer(
        &pData,
        &nNumFramesToRead,
        &dwFlags,
        NULL,
        NULL
      );
      if(FAILED(hr)) {
        puts("getBuffer失敗した");
        return 0;
      }
      // ringBufferから今回書き込むべきbufferをもらう。
      ttLibC_Buffer *buffer = ringBuffer.getAddBuffer();
      if(buffer == NULL) {
//        puts("bufferが準備されてません。");
      }
      else {
        uint8_t *pcm = buffer->pcm; // ここにデータをコピーしなければならない。
        if(buffer->length < 524288) { // データが溜まりすぎている場合は処理するとオーバーフローするので一応確認しておく。
          size_t size = nNumFramesToRead * pwfx->nBlockAlign;
          pcm += buffer->length;
          memcpy(pcm, pData, size);
          buffer->length += size;
        }
        else {
          puts("bufferのデータがあふれるので、データをドロップします。");
        }
      }
      // 今回参照したbufferを解放する(でないと次のデータ取得できない)
      hr = pAudioCaptureClient->ReleaseBuffer(nNumFramesToRead);
      if(FAILED(hr)) {
        puts("releaseBuffer失敗");
        return 0;
      }
    }
    // waitTimerのシグナル待ちを実施することで正確にsleepする
    if(WaitForSingleObject(hWakeUp, INFINITE) != WAIT_OBJECT_0) {
      puts("timer待ちでエラーがでた。");
      return 0;
    }
  }
  return 0;
}

/**
 * デバイス一覧を取得してcallbackしようと思ってた。
 * まだ利用していない。
 * 一覧から名前を取得して、その名前をいれて初期化すると、同一のdeviceのcaptureを試すみたいな形にしたいと思ってる。
 */
bool ttLibC_MmAudioLoopback_getDeviceNames(
    const char *locale,
    ttLibC_MmAudioLoopbackDeviceNameFunc callback,
    void *ptr) {
  bool result = true;
  if(locale != NULL) {
    setlocale(LC_ALL, locale);
  }
  HRESULT hr = S_OK;
  hr = CoInitialize(NULL);
  if(FAILED(hr)) {
    return false;
  }
  CoUninitializeOnExit cuoe;
  IMMDeviceEnumerator *pMMDeviceEnumerator = NULL;
  hr = CoCreateInstance(
    __uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
    __uuidof(IMMDeviceEnumerator),
    (void **)&pMMDeviceEnumerator
  );
  if(FAILED(hr)) {
    return false;
  }
  ReleaseOnExit roeDeviceEnumerator(pMMDeviceEnumerator);
  IMMDeviceCollection *pMMDeviceCollection = NULL;
  hr = pMMDeviceEnumerator->EnumAudioEndpoints(
    eRender, DEVICE_STATE_ACTIVE, &pMMDeviceCollection
  );
  if(FAILED(hr)) {
    return false;
  }
  ReleaseOnExit roeDeviceCollection(pMMDeviceCollection);

  uint32_t deviceNum = 0;
  hr = pMMDeviceCollection->GetCount(&deviceNum);
  if(FAILED(hr)) {
    return false;
  }
  for(uint32_t i = 0;i < deviceNum;++ i) {
    IMMDevice *pMMDevice = NULL;
    hr = pMMDeviceCollection->Item(i, &pMMDevice);
    if(FAILED(hr)) {
      return false;
    }
    ReleaseOnExit roeDevice(pMMDevice);
    IPropertyStore *pPropertyStore = NULL;
    hr = pMMDevice->OpenPropertyStore(STGM_READ, &pPropertyStore);
    if(FAILED(hr)) {
      return false;
    }
    ReleaseOnExit roePropertyStore(pPropertyStore);
    PROPVARIANT pv;
    PropVariantInit(&pv);
    hr = pPropertyStore->GetValue(PKEY_Device_FriendlyName, &pv);
    if(FAILED(hr)) {
      return false;
    }
    PropVariantClearOnExit pvcoe(&pv);
    if(pv.vt != VT_LPWSTR) {
      continue;
    }
    char buf[256];
    size_t size;
    wcstombs_s(
      &size,
      buf,
      (wcslen(pv.pwszVal) + 1) * sizeof(wchar_t),
      pv.pwszVal,
      (wcslen(pv.pwszVal) + 1) * sizeof(wchar_t)
    );
    if(callback != NULL) {
      if(!callback(ptr, buf)) {
        return false;
      }
    }
  }
  return true;
}

/**
 * audioLoopbackを作成する
 */
ttLibC_MmAudioLoopback *ttLibC_MmAudioLoopback_make(
    const char *locale,
    const char *deviceName) {
  // オブジェクト作成
  ttLibC_MmAudioLoopback_ *loopback = (ttLibC_MmAudioLoopback_ *)ttLibC_malloc(sizeof(ttLibC_MmAudioLoopback_));
  if(loopback == NULL) {
    return NULL;
  }
  memset(loopback, 0, sizeof(ttLibC_MmAudioLoopback_));
  // locale設定されてるなら、コピーしておく
  if(locale == NULL){
    strcpy(loopback->locale, "");
  }
  else {
    strcpy(loopback->locale, locale);
  }
  // デバイス名も同じ
  if(deviceName == NULL) {
    strcpy(loopback->deviceName, "");
  }
  else {
    strcpy(loopback->deviceName, deviceName);
  }
  loopback->is_running = false;
  // ringBufferはthreadの内部でつくって管理する
  // threadをつくって、そっちでやってもらう
  loopback->hThread = CreateThread(
    NULL, 0,
    MmAudioLoopback_threadCallback,
    loopback,
    0,
    NULL
  );
  return (ttLibC_MmAudioLoopback *)loopback;
}

/**
 * frameをloopbackから取り出す
 */
bool ttLibC_MmAudioLoopback_queryFrame(
    ttLibC_MmAudioLoopback *device,
    ttLibC_MmAudioLoopbackFrameFunc callback,
    void *ptr) {
  ttLibC_MmAudioLoopback_ *loopback = (ttLibC_MmAudioLoopback_ *)device;
  if(loopback == NULL) {
    return false;
  }
  if(loopback->is_running) {
    ttLibC_Buffer *buffer = loopback->ringBuffer->getRefBuffer();
    loopback->ringBuffer->prepareNextBuffer();
    if(buffer == NULL) {
//      puts("bufferがまだない。");
    }
    else {
      // このデータをpcmとして取り出すぜ
      uint8_t *pcm = buffer->pcm;
      uint8_t *r_data = NULL;
      size_t r_size = 0;
      uint32_t sample_num = (buffer->length >> 1);
      if(loopback->channel_num == 2) {
        r_data = pcm + 1;
        r_size = buffer->length;
        sample_num = (buffer->length >> 2);
      }
      // ttLibC_Frameにする。
      ttLibC_PcmS16 *p = ttLibC_PcmS16_make(
        NULL,
        PcmS16Type_littleEndian,
        loopback->sample_rate,
        sample_num,
        loopback->channel_num,
        pcm,
        buffer->length,
        pcm,
        buffer->length,
        r_data,
        r_size,
        true,
        loopback->totalSampleNum,
        loopback->sample_rate);
      loopback->totalSampleNum += sample_num;
      if(p != NULL) {
        if(callback != NULL) {
          callback(ptr, (ttLibC_Frame *)p);
        }
        ttLibC_PcmS16_close(&p);
      }
      loopback->ringBuffer->releaseBuffer(buffer);
    }
  }
  return true;
}

/**
 * 処理終わり
 */
void ttLibC_MmAudioLoopback_close(ttLibC_MmAudioLoopback **device) {
  ttLibC_MmAudioLoopback_ *target = (ttLibC_MmAudioLoopback_ *)*device;
  if(target == NULL) {
    return;
  }
  // とりあえずCoUninitializeしとく。
  if(target->is_running) {
    target->is_running = false;
  }
  if(target->hThread != NULL) {
    CloseHandle(target->hThread);
    target->hThread = NULL;
  }
  ttLibC_free(target);
  *device = NULL;
}

#endif

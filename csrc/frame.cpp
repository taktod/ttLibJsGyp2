#include "predef.h"
#include "frame.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <ttLibC/allocator.h>
#include <ttLibC/log.h>
#include <ttLibC/frame/audio/audio.h>
#include <ttLibC/frame/audio/aac.h>
#include <ttLibC/frame/audio/adpcmImaWav.h>
#include <ttLibC/frame/audio/mp3.h>
#include <ttLibC/frame/audio/nellymoser.h>
#include <ttLibC/frame/audio/opus.h>
#include <ttLibC/frame/audio/pcmAlaw.h>
#include <ttLibC/frame/audio/pcmf32.h>
#include <ttLibC/frame/audio/pcmMulaw.h>
#include <ttLibC/frame/audio/pcms16.h>
#include <ttLibC/frame/audio/speex.h>
#include <ttLibC/frame/audio/vorbis.h>
#include <ttLibC/frame/video/video.h>
#include <ttLibC/frame/video/bgr.h>
#include <ttLibC/frame/video/flv1.h>
#include <ttLibC/frame/video/h264.h>
#include <ttLibC/frame/video/h265.h>
#include <ttLibC/frame/video/jpeg.h>
#include <ttLibC/frame/video/theora.h>
#include <ttLibC/frame/video/vp6.h>
#include <ttLibC/frame/video/vp8.h>
#include <ttLibC/frame/video/vp9.h>
#include <ttLibC/frame/video/wmv1.h>
#include <ttLibC/frame/video/wmv2.h>
#include <ttLibC/frame/video/yuv420.h>

#include <ttLibC/util/hexUtil.h>

void TTLIBJSGYP_CDECL Frame::classInit(Local<Object> target) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Frame").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  SetPrototypeMethod(tpl, "getBinaryBuffer", GetBinaryBuffer);
  SetPrototypeMethod(tpl, "clone", Clone);
  Local<Function> func = Nan::GetFunction(tpl).ToLocalChecked();
  constructor().Reset(func);
  Nan::Set(func, Nan::New("fromBinaryBuffer").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(FromBinaryBuffer)).ToLocalChecked());
  Nan::Set(
    target,
    Nan::New("Frame").ToLocalChecked(),
    func);
}

Local<Object> Frame::newInstance() {
  return Nan::NewInstance(
    Nan::New(constructor()),
    0,
    NULL).ToLocalChecked();
}

bool Frame::setFrame(Local<Object> jsFrame, ttLibC_Frame *ttFrame) {
  // ここでframeの内容を更新したりしないといけない。
  // 内包しているFrameオブジェクトとjsFrameの内容とttFrameの内容の３つが一致していなければならない。
  // この動作はjsFrameの内容を上書きして、内部で保持しているttLibC_Frameを更新することが目的
  /*
  type
  width
  height
  videoType key inner info

  sampleRate
  sampleNum
  channelNum

  frameType フレームごとに特殊な値
  dataPos 開始位置調整
  stride  stride調整
  */
  // jsFrameが保持しているid timebase ptsを参照しなければならない。
  // 変更している場合は、保持しているデータが更新する。
  // とりあえずはじめは全部更新でつくっておくか
  if(ttFrame->pts < 0xFFFFFFFFL) {
    Nan::Set(jsFrame, Nan::New("pts").ToLocalChecked(), Nan::New((uint32_t)ttFrame->pts));
  }
  else {
    Nan::Set(jsFrame, Nan::New("pts").ToLocalChecked(), Nan::New((double)ttFrame->pts));
  }
  Nan::Set(jsFrame, Nan::New("timebase").ToLocalChecked(), Nan::New(ttFrame->timebase));
  Nan::Set(jsFrame, Nan::New("id").ToLocalChecked(),       Nan::New(ttFrame->id));
  if(ttLibC_Frame_isAudio(ttFrame)) {
    ttLibC_Audio *audio = (ttLibC_Audio *)ttFrame;
    Nan::Set(jsFrame, Nan::New("sampleRate").ToLocalChecked(), Nan::New(audio->sample_rate));
    Nan::Set(jsFrame, Nan::New("sampleNum").ToLocalChecked(),  Nan::New(audio->sample_num));
    Nan::Set(jsFrame, Nan::New("channelNum").ToLocalChecked(), Nan::New(audio->channel_num));
    switch(ttFrame->type) {
    case frameType_aac:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("aac").ToLocalChecked());
      break;
    case frameType_adpcm_ima_wav:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("adpcmImaWav").ToLocalChecked());
      break;
    case frameType_mp3:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("mp3").ToLocalChecked());
      break;
    case frameType_nellymoser:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("nellymoser").ToLocalChecked());
      break;
    case frameType_opus:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("opus").ToLocalChecked());
      break;
    case frameType_pcm_alaw:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("pcmAlaw").ToLocalChecked());
      break;
    case frameType_pcmF32:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("pcmF32").ToLocalChecked());
      break;
    case frameType_pcm_mulaw:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("pcmMulaw").ToLocalChecked());
      break;
    case frameType_pcmS16:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("pcmS16").ToLocalChecked());
      break;
    case frameType_speex:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("speex").ToLocalChecked());
      break;
    case frameType_vorbis:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("vorbis").ToLocalChecked());
      break;
    default:
      puts("不明なフレームタイプでした。");
      return false;
    }
  }
  else if(ttLibC_Frame_isVideo(ttFrame)) {
    ttLibC_Video *video = (ttLibC_Video *)ttFrame;
    Nan::Set(jsFrame, Nan::New("width").ToLocalChecked(),  Nan::New(video->width));
    Nan::Set(jsFrame, Nan::New("height").ToLocalChecked(), Nan::New(video->height));
    switch(video->type) {
    case videoType_key:
      Nan::Set(jsFrame, Nan::New("videoType").ToLocalChecked(), Nan::New("key").ToLocalChecked());
      break;
    case videoType_inner:
      Nan::Set(jsFrame, Nan::New("videoType").ToLocalChecked(), Nan::New("inner").ToLocalChecked());
      break;
    case videoType_info:
      Nan::Set(jsFrame, Nan::New("videoType").ToLocalChecked(), Nan::New("info").ToLocalChecked());
      break;
    default:
      break;
    }
    switch(ttFrame->type) {
    case frameType_bgr:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("bgr").ToLocalChecked());
      break;
    case frameType_flv1:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("flv1").ToLocalChecked());
      break;
    case frameType_h264:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("h264").ToLocalChecked());
      break;
    case frameType_h265:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("h265").ToLocalChecked());
      break;
    case frameType_jpeg:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("jpeg").ToLocalChecked());
      break;
    case frameType_theora:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("theora").ToLocalChecked());
      break;
    case frameType_vp6:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("vp6").ToLocalChecked());
      break;
    case frameType_vp8:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("vp8").ToLocalChecked());
      break;
    case frameType_vp9:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("vp9").ToLocalChecked());
      break;
    case frameType_wmv1:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("wmv1").ToLocalChecked());
      break;
    case frameType_wmv2:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("wmv2").ToLocalChecked());
      break;
    case frameType_yuv420:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("yuv").ToLocalChecked());
      break;
    default:
      puts("不明なフレームタイプでした。");
      return false;
    }
  }
  else {
    puts("映像でも音声でもないフレームでした。");
    return false;
  }
  Frame *frame = Nan::ObjectWrap::Unwrap<Frame>(jsFrame);
  frame->frame_ = ttFrame;
  frame->isRef_ = true;
  return true;
}

ttLibC_Frame_Type Frame::getFrameType(std::string name) {
  if(name == "aac") {
    return frameType_aac;
  }
  else if(name == "adpcmImaWav") {
    return frameType_adpcm_ima_wav;
  }
  else if(name == "mp3") {
    return frameType_mp3;
  }
  else if(name == "nellymoser") {
    return frameType_nellymoser;
  }
  else if(name == "opus") {
    return frameType_opus;
  }
  else if(name == "pcmAlaw") {
    return frameType_pcm_alaw;
  }
  else if(name == "pcmF32") {
    return frameType_pcmF32;
  }
  else if(name == "pcmMulaw") {
    return frameType_pcm_mulaw;
  }
  else if(name == "pcmS16") {
    return frameType_pcmS16;
  }
  else if(name == "speex") {
    return frameType_speex;
  }
  else if(name == "vorbis") {
    return frameType_vorbis;
  }
  else if(name == "bgr") {
    return frameType_bgr;
  }
  else if(name == "flv1") {
    return frameType_flv1;
  }
  else if(name == "h264") {
    return frameType_h264;
  }
  else if(name == "h265") {
    return frameType_h265;
  }
  else if(name == "jpeg") {
    return frameType_jpeg;
  }
  else if(name == "theora") {
    return frameType_theora;
  }
  else if(name == "vp6") {
    return frameType_vp6;
  }
  else if(name == "vp8") {
    return frameType_vp8;
  }
  else if(name == "vp9") {
    return frameType_vp9;
  }
  else if(name == "wmv1") {
    return frameType_wmv1;
  }
  else if(name == "wmv2") {
    return frameType_wmv2;
  }
  else if(name == "yuv") {
    return frameType_yuv420;
  }
  return frameType_unknown;
}

ttLibC_Frame *Frame::refFrame(Local<Value> jsVFrame) {
  if(!jsVFrame->IsObject()) {
    return NULL;
  }
  if(jsVFrame->IsArrayBuffer() || jsVFrame->IsTypedArray()) {
    return NULL;
  }
  Local<Object> jsFrame = jsVFrame->ToObject();

  Frame *frame = Nan::ObjectWrap::Unwrap<Frame>(jsFrame);
  Local<Value> id       = Nan::Get(jsFrame, Nan::New("id").ToLocalChecked()).ToLocalChecked();
  Local<Value> pts      = Nan::Get(jsFrame, Nan::New("pts").ToLocalChecked()).ToLocalChecked();
  Local<Value> timebase = Nan::Get(jsFrame, Nan::New("timebase").ToLocalChecked()).ToLocalChecked();
  frame->frame_->id = id->Uint32Value();
  frame->frame_->pts = (uint64_t)pts->NumberValue();
  frame->frame_->timebase = timebase->Uint32Value();
  // まだまだ追加すべきデータがあるはず。strideとかdataPosとか
  return frame->frame_;
}

NAN_METHOD(Frame::New) {
  /*
  何もないところからつくって　-> ttLibC_Frameを追加して扱う
  JsのFrameオブジェクトから、内部のttLibC_Frameのcloneをつくって、それを利用するものを作っておいたら幸せになれそうではある。
  コンストラクタではなくて、cloneというメソッドを準備しておく方がいいかな。
  BinaryデータからFrameオブジェクトを復元する動作もほしい。
  これもFrameを空から再生成して・・・とした方がいいかね。
   */
  if(info.IsConstructCall()) {
    Frame *frame = new Frame();
    frame->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  }
  else {
    info.GetReturnValue().Set(newInstance());
  }
}

NAN_METHOD(Frame::GetBinaryBuffer) {
  Frame *frame = Nan::ObjectWrap::Unwrap<Frame>(info.Holder());
  if(frame->frame_ == NULL) {
    info.GetReturnValue().Set(Nan::Null());
    return;
  }
  info.GetReturnValue().Set(Nan::CopyBuffer((char *)frame->frame_->data, frame->frame_->buffer_size).ToLocalChecked());
}

/**
 * binaryStringからJsFrameを復元します
 * @param prevFrame 前回つかった同じ系列のデータ
 * @param buffer    frameにしたいbinaryデータ
 * @param params    その他追加データ 連想配列で設定
 * @return Frameデータ、失敗時にはnullが応答される。
 *
 *  paramsの内容
 *   type: フレームのタイプを文字列で指定
 *   id: フレームのIDを数値で指定
 *   pts: フレームのptsを数値で指定
 *   timebase: フレームのtimebaseを数値で指定
 *   ここまでが基本
 *   subType: pcmS16、pcmF32、bgr、yuvの場合の詳細設定をいれる。
 *   width: 横幅
 *   height: 縦幅
 *   stride: bgrのstride値
 *   yStride: yuvのy要素のstride値
 *   uStride: yuvのu要素のstride値
 *   vStride: yuvのv要素のstride値
 *   sampleRate: サンプルレート pcmS16 pcmF32 nellymoser pcmAlaw pcmMulaw adpcmImaWav等で利用する
 *   channelNum: チャンネル数 pcmS16 pcmF32 nellymoser pcmAlaw pcmMulaw adpcmImaWav等で利用する
 *   sampleNum: サンプル数 pcmS16 pcmF32 nellymoser pcmAlaw pcmMulaw adpcmImaWav等で利用する
 *   adjustment: vp6のadjustmentデータ
 *   videoType: wmv1 wmv2で利用するkey inner infoの指定
 * といった具合
 */
NAN_METHOD(Frame::FromBinaryBuffer) {
#define GetParamInt(target) do { \
  Local<Value> Js##target = Nan::Get(params, Nan::New(#target).ToLocalChecked()).ToLocalChecked(); \
  if(Js##target->IsNumber() || Js##target->IsUint32() || Js##target->IsInt32()) { \
    target = Js##target->Uint32Value(); \
  } \
} while(0)
#define GetParamLong(target) do { \
  Local<Value> Js##target = Nan::Get(params, Nan::New(#target).ToLocalChecked()).ToLocalChecked(); \
  if(Js##target->IsNumber() || Js##target->IsUint32() || Js##target->IsInt32()) { \
    target = (uint64_t)Js##target->NumberValue(); \
  } \
} while(0)
#define GetParamString(target) do { \
  Local<Value> Js##target = Nan::Get(params, Nan::New(#target).ToLocalChecked()).ToLocalChecked(); \
  if(Js##target->IsString()) { \
    target = std::string(*String::Utf8Value(Js##target->ToString())); \
  } \
} while(0)
  // binaryBuffer(ArrayBufferやTypedDataView、Buffer)からttLibC_Frameを復元する動作
  // 基本Cloneと同じ
  // prevFrame, buffer, paramsとしよう。
  // paramsの中にtype, id, pts, timebaseの基本情報や
  // prevFrameの内容を継承できるものは、そっちを優先するということで・・・
  // y_strideとかsubTypeとかいろいろいれるべき
  if(info.Length() < 2) {
    // info.Lengthが2未満の場合は動作不能。
    ERR_PRINT("入力パラメーターが足りません");
    info.GetReturnValue().Set(Nan::Null());
    return;
  }
  ttLibC_Frame *prevFrame = refFrame(info[0]);
  Local<Object> params;
  std::string type("");
  ttLibC_Frame_Type frameType;
  uint8_t *data;
  size_t data_size;
  uint64_t pts = 0;
  uint32_t timebase = 1000;
  uint32_t id = 0;
  if(!info[1]->IsTypedArray() && !info[1]->IsArrayBuffer()) {
    ERR_PRINT("fromBinaryBufferの入力データが不正です。");
    info.GetReturnValue().Set(Nan::Null());
    return;
  }
  // とりあえずデフォルトの動作として、prevFrameの内容を踏襲する。
  // 可能な限りではあるが・・・
  if(prevFrame != NULL) {
    frameType = prevFrame->type;
    id        = prevFrame->id;
    timebase  = prevFrame->timebase;
  }
  data = (uint8_t *)node::Buffer::Data(info[1]->ToObject());
  data_size = node::Buffer::Length(info[1]->ToObject());
  if(info.Length() <= 3 && info[2]->IsObject()) {
    // 2つ目のパラメーターが連想配列であるとして処理しておく
    params = info[2]->ToObject();
  }
  else {
    params = Nan::New<Object>();
  }
  GetParamString(type);
  if(type != "") {
    // frameTypeを更新
    frameType = getFrameType(type);
  }
  // とりあえず基本
  GetParamInt(id);
  GetParamLong(pts);
  GetParamInt(timebase);
  ttLibC_Frame *frame = NULL;
  switch(frameType) {
  case frameType_aac:
    {
      frame = (ttLibC_Frame *)ttLibC_Aac_getFrame(
          (ttLibC_Aac *)prevFrame,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_adpcm_ima_wav:
    {
      uint32_t sampleRate = 0;
      uint32_t sampleNum = 0;
      uint32_t channelNum = 1;
      GetParamInt(sampleRate);
      GetParamInt(channelNum);
      GetParamInt(sampleNum);
      if(sampleRate == 0) {
        ERR_PRINT("sampleRateが未設定です。nellymoser復元できません。");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      frame = (ttLibC_Frame *)ttLibC_AdpcmImaWav_make(
          (ttLibC_AdpcmImaWav *)prevFrame,
          sampleRate,
          sampleNum,
          channelNum,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_mp3:
    {
      frame = (ttLibC_Frame *)ttLibC_Mp3_getFrame(
          (ttLibC_Mp3 *)prevFrame,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_nellymoser:
    {
      uint32_t sampleRate = 0;
      uint32_t sampleNum = 0;
      uint32_t channelNum = 1;
      GetParamInt(sampleRate);
      GetParamInt(channelNum);
      if(sampleRate == 0) {
        ERR_PRINT("sampleRateが未設定です。nellymoser復元できません。");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      GetParamInt(sampleNum);
      // こっちはsampleRateはどうなるか不明だけど、仮として44100をいれておく。
      // channelは1
      // あとでjsFrameの属性を変更したら、それを反映するようにすればよい。
      frame = (ttLibC_Frame *)ttLibC_Nellymoser_make(
          (ttLibC_Nellymoser *)prevFrame,
          sampleRate,
          sampleNum,
          channelNum,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_opus:
    {
      frame = (ttLibC_Frame *)ttLibC_Opus_getFrame(
          (ttLibC_Opus *)prevFrame,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_pcm_alaw:
    {
      uint32_t sampleRate = 0;
      uint32_t channelNum = 1;
      GetParamInt(sampleRate);
      GetParamInt(channelNum);
      if(sampleRate == 0) {
        ERR_PRINT("sampleRateが未設定です。pcmAlaw復元できません。");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      // 仮でsampleRate = 16000、channelNum = 1で設定して構築しておく。
      frame = (ttLibC_Frame *)ttLibC_PcmAlaw_make(
          (ttLibC_PcmAlaw *)prevFrame,
          sampleRate,
          data_size,
          channelNum,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_pcmF32:
    {
      std::string subType = "interleave";
      uint32_t sampleRate = 0;
      uint32_t channelNum = 1;
      if(sampleRate == 0) {
        ERR_PRINT("sampleRateが未設定です。pcmF32復元できません。");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      uint32_t sampleNum = data_size / channelNum / sizeof(float);
      GetParamInt(sampleNum);
      ttLibC_PcmF32_Type frameSubType = PcmF32Type_interleave;
      uint8_t *newData = (uint8_t *)ttLibC_malloc(data_size);
      if(newData == NULL) {
        ERR_PRINT("memory alloc失敗しました。");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      memcpy(newData, data, data_size);
      uint8_t *lData = newData;
      uint8_t *rData = NULL;
      size_t lStride = data_size;
      size_t rStride = 0;
      if(subType == "interleave") {
        frameSubType = PcmF32Type_interleave;
        lStride = sampleNum * 4 * channelNum;
        GetParamInt(lStride);
        rData = NULL;
        rStride = 0;
      }
      else if(subType == "planar") {
        frameSubType = PcmF32Type_planar;
        lStride = sampleNum * 4;
        GetParamInt(lStride);
        if(channelNum == 2) {
          rData = lData + lStride;
          rStride = lStride;
          GetParamInt(rStride);
        }
      }
      frame = (ttLibC_Frame *)ttLibC_PcmF32_make(
          (ttLibC_PcmF32 *)prevFrame,
          frameSubType,
          sampleRate,
          sampleNum,
          channelNum,
          newData,
          data_size,
          lData,
          lStride,
          rData,
          rStride,
          true,
          pts,
          timebase);
      if(frame != NULL) {
        frame->is_non_copy = false;
      }
    }
    break;
  case frameType_pcm_mulaw:
    {
      uint32_t sampleRate = 0;
      uint32_t channelNum = 1;
      GetParamInt(sampleRate);
      GetParamInt(channelNum);
      if(sampleRate == 0) {
        ERR_PRINT("sampleRateが未設定です。pcmMulaw復元できません。");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      // こっちも仮でsampleRate = 16000、channelNum = 1で設定する。
      frame = (ttLibC_Frame *)ttLibC_PcmMulaw_make(
          (ttLibC_PcmMulaw *)prevFrame,
          sampleRate,
          data_size,
          channelNum,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_pcmS16:
    {
      std::string subType = "littleEndian";
      uint32_t sampleRate = 0;
      uint32_t channelNum = 1;
      if(sampleRate == 0) {
        ERR_PRINT("sampleRateが未設定です。pcmS16復元できません。");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      uint32_t sampleNum = data_size / channelNum / sizeof(int16_t);
      GetParamInt(sampleNum);
      ttLibC_PcmS16_Type frameSubType = PcmS16Type_littleEndian;
      uint8_t *newData = (uint8_t *)ttLibC_malloc(data_size);
      if(newData == NULL) {
        ERR_PRINT("memory alloc失敗しました。");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      memcpy(newData, data, data_size);
      uint8_t *lData = newData;
      uint8_t *rData = NULL;
      size_t lStride = data_size;
      size_t rStride = 0;
      if(subType == "littleEndian") {
        frameSubType = PcmS16Type_littleEndian;
        lStride = sampleNum * 2 * channelNum;
        GetParamInt(lStride);
        rData = NULL;
        rStride = 0;
      }
      else if(subType == "littleEndianPlanar") {
        frameSubType = PcmS16Type_littleEndian_planar;
        lStride = sampleNum * 2;
        GetParamInt(lStride);
        if(channelNum == 2) {
          rData = lData + lStride;
          rStride = lStride;
          GetParamInt(rStride);
        }
      }
      else if(subType == "bigEndian") {
        frameSubType = PcmS16Type_bigEndian;
        lStride = sampleNum * 2 * channelNum;
        GetParamInt(lStride);
        rData = NULL;
        rStride = 0;
      }
      else if(subType == "bigEndianPlanar") {
        frameSubType = PcmS16Type_bigEndian_planar;
        lStride = sampleNum * 2;
        GetParamInt(lStride);
        if(channelNum == 2) {
          rData = lData + lStride;
          rStride = lStride;
          GetParamInt(rStride);
        }
      }
      frame = (ttLibC_Frame *)ttLibC_PcmS16_make(
          (ttLibC_PcmS16 *)prevFrame,
          frameSubType,
          sampleRate,
          sampleNum,
          channelNum,
          newData,
          data_size,
          lData,
          lStride,
          rData,
          rStride,
          true,
          pts,
          timebase);
      if(frame != NULL) {
        frame->is_non_copy = false;
      }
    }
    break;
  case frameType_speex:
    {
      frame = (ttLibC_Frame *)ttLibC_Speex_getFrame(
          (ttLibC_Speex *)prevFrame,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_vorbis:
    {
      frame = (ttLibC_Frame *)ttLibC_Vorbis_getFrame(
          (ttLibC_Vorbis *)prevFrame,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;

  case frameType_bgr:
    {
      std::string subType = "bgr";
      uint32_t width  = 0;
      uint32_t height = 0;
      GetParamInt(width);
      GetParamInt(height);
      if(width == 0 || height == 0) {
        ERR_PRINT("widthまたはheightが未設定です。bgr復元できません");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      uint32_t stride = width * 3;
      GetParamString(subType);
      ttLibC_Bgr_Type frameSubType = BgrType_bgr;
      if(subType == "bgr") {
        GetParamInt(stride);
        frameSubType = BgrType_bgr;
      }
      else if(subType == "bgra") {
        stride = width * 4;
        GetParamInt(stride);
        frameSubType = BgrType_bgra;
      }
      else if(subType == "abgr") {
        stride = width * 4;
        GetParamInt(stride);
        frameSubType = BgrType_abgr;
      }
      frame = (ttLibC_Frame *)ttLibC_Bgr_make(
          (ttLibC_Bgr *)prevFrame,
          frameSubType,
          width,
          height,
          stride,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_flv1:
    {
      frame = (ttLibC_Frame *)ttLibC_Flv1_getFrame(
          (ttLibC_Flv1 *)prevFrame,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_h264:
    {
      frame = (ttLibC_Frame *)ttLibC_H264_getFrame(
          (ttLibC_H264 *)prevFrame,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_h265:
    {
      frame = (ttLibC_Frame *)ttLibC_H265_getFrame(
          (ttLibC_H265 *)prevFrame,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_jpeg:
    {
      frame = (ttLibC_Frame *)ttLibC_Jpeg_getFrame(
          (ttLibC_Jpeg *)prevFrame,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_theora:
    {
      frame = (ttLibC_Frame *)ttLibC_Theora_getFrame(
          (ttLibC_Theora *)prevFrame,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_vp6:
    {
      // adjustmentはいれないけど、あとでframeのwidthを設定すれば、そっちを採用するという形で攻めればいいと思う
      uint32_t adjustment = 0x00;
      GetParamInt(adjustment);
      frame = (ttLibC_Frame *)ttLibC_Vp6_getFrame(
          (ttLibC_Vp6 *)prevFrame,
          data,
          data_size,
          false,
          pts,
          timebase,
          adjustment);
    }
    break;
  case frameType_vp8:
    {
      frame = (ttLibC_Frame *)ttLibC_Vp8_getFrame(
          (ttLibC_Vp8 *)prevFrame,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_vp9:
    {
      frame = (ttLibC_Frame *)ttLibC_Vp9_getFrame(
          (ttLibC_Vp9 *)prevFrame,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_wmv1:
    {
      uint32_t width  = 0;
      uint32_t height = 0;
      std::string videoType = "key";
      GetParamInt(width);
      GetParamInt(height);
      if(width == 0 || height == 0) {
        ERR_PRINT("widthまたはheightが未設定です。wmv1復元できません");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      GetParamString(videoType);
      ttLibC_Video_Type video_type = videoType_key;
      if(videoType == "key") {
        video_type = videoType_key;
      }
      else if(videoType == "inner") {
        video_type = videoType_inner;
      }
      else if(videoType == "info") {
        video_type = videoType_info;
      }
      frame = (ttLibC_Frame *)ttLibC_Wmv1_make(
          (ttLibC_Wmv1 *)prevFrame,
          video_type,
          width,
          height,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_wmv2:
    {
      uint32_t width  = 0;
      uint32_t height = 0;
      std::string videoType = "key";
      GetParamInt(width);
      GetParamInt(height);
      if(width == 0 || height == 0) {
        ERR_PRINT("widthまたはheightが未設定です。wmv2復元できません");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      GetParamString(videoType);
      ttLibC_Video_Type video_type = videoType_key;
      if(videoType == "key") {
        video_type = videoType_key;
      }
      else if(videoType == "inner") {
        video_type = videoType_inner;
      }
      else if(videoType == "info") {
        video_type = videoType_info;
      }
      frame = (ttLibC_Frame *)ttLibC_Wmv2_make(
          (ttLibC_Wmv2 *)prevFrame,
          video_type,
          width,
          height,
          data,
          data_size,
          false,
          pts,
          timebase);
    }
    break;
  case frameType_yuv420:
    {
      std::string subType = "planar";
      uint32_t width  = 0;
      uint32_t height = 0;
      GetParamInt(width);
      GetParamInt(height);
      if(width == 0 || height == 0) {
        ERR_PRINT("widthまたはheightが未設定です。yuv420復元できません");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      uint32_t yStride = width;
      uint32_t uStride = (width >> 1);
      uint32_t vStride = (width >> 1);

      GetParamString(subType);
      ttLibC_Yuv420_Type frameSubType = Yuv420Type_planar;
      uint8_t *newData = (uint8_t *)ttLibC_malloc(data_size);
      if(newData == NULL) {
        ERR_PRINT("memory alloc失敗しました。");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      memcpy(newData, data, data_size);
      uint8_t *yData = newData;
      uint8_t *uData = newData;
      uint8_t *vData = newData;
      if(subType == "planar") {
        GetParamInt(yStride);
        GetParamInt(uStride);
        GetParamInt(vStride);
        uData = yData + height * yStride;
        vData = uData + (height >> 1) * uStride;
      }
      else if(subType == "semiPlanar") {
        uStride = width;
        vStride = width;
        GetParamInt(yStride);
        GetParamInt(uStride);
        GetParamInt(vStride);
        frameSubType = Yuv420Type_semiPlanar;
        uData = yData + height * yStride;
        vData = uData + 1;
      }
      else if(subType == "yvuPlanar") {
        GetParamInt(yStride);
        GetParamInt(uStride);
        GetParamInt(vStride);
        frameSubType = Yvu420Type_planar;
        vData = yData + height * yStride;
        uData = vData + (height >> 1) * vStride;
      }
      else if(subType == "yvuSemiPlanar") {
        uStride = width;
        vStride = width;
        GetParamInt(yStride);
        GetParamInt(uStride);
        GetParamInt(vStride);
        frameSubType = Yvu420Type_semiPlanar;
        vData = yData + height * yStride;
        uData = vData + 1;
      }
      else {
        ERR_PRINT("subTypeが不正です。yuv420復元できません");
        info.GetReturnValue().Set(Nan::Null());
        return;
      }
      frame = (ttLibC_Frame *)ttLibC_Yuv420_make(
          (ttLibC_Yuv420 *)prevFrame,
          frameSubType,
          width,
          height,
          newData,
          data_size,
          yData,
          yStride,
          uData,
          uStride,
          vData,
          vStride,
          true,
          pts,
          timebase);
      if(frame != NULL) {
        frame->is_non_copy = false;
      }
    }
    break;
  default:
    break;
  }
  if(frame == NULL) {
    ERR_PRINT("frameが生成されませんでした。");
    info.GetReturnValue().Set(Nan::Null());
    return;
  }
  frame->id = id;
  Local<Object> jsFrame;
  if(prevFrame == NULL) {
    jsFrame = Frame::newInstance();
  }
  else {
    jsFrame = info[0]->ToObject();
  }
  Frame::setFrame(jsFrame, frame);
  Frame *newFrame = Nan::ObjectWrap::Unwrap<Frame>(jsFrame);
  newFrame->isRef_ = false;
  info.GetReturnValue().Set(jsFrame);
#undef GetParamInt
#undef GetParamLong
#undef GetParamString
}

NAN_METHOD(Frame::Clone) {
  Frame *frame = Nan::ObjectWrap::Unwrap<Frame>(info.Holder());
  Local<Object> jsFrame = Frame::newInstance();
  Frame::setFrame(jsFrame, ttLibC_Frame_clone(NULL, frame->frame_));
  Frame *newFrame = Nan::ObjectWrap::Unwrap<Frame>(jsFrame);
  newFrame->isRef_ = false;
  info.GetReturnValue().Set(jsFrame);
}

Frame::Frame() {
  frame_ = NULL;
  isRef_ = false;
}

Frame::~Frame() {
  if(!isRef_) {
    ttLibC_Frame_close(&frame_);
  }
}

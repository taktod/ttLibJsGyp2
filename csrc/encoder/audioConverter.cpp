#include "audioConverter.h"
#include "../frame.h"

class FramePassingWorker : public Nan::AsyncWorker {
public:
  FramePassingWorker(
      ttLibC_Frame *frame,
      AudioConverterEncoder *encoder) :
        Nan::AsyncWorker(NULL),
        encoder_(encoder) {
    frame_ = ttLibC_Frame_clone(NULL, frame);
  }
  void Execute() {

  }
  void HandleOKCallback() {
    encoder_->asyncCallback(frame_);
  }
private:
  ~FramePassingWorker() {
    ttLibC_Frame_close(&frame_);
  }
  AudioConverterEncoder *encoder_;
  ttLibC_Frame *frame_;
};

AudioConverterEncoder::AudioConverterEncoder(Local<Object> params) : Encoder() {
  type_ = get_audioConverter;
#ifdef __ENABLE_APPLE__
  ttLibC_Frame_Type frameType = Frame::getFrameType(
    std::string(*String::Utf8Value(
      Nan::Get(params, Nan::New("type").ToLocalChecked()).ToLocalChecked()->ToString()))
  );
  uint32_t sampleRate = Nan::Get(params, Nan::New("sampleRate").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t channelNum = Nan::Get(params, Nan::New("channelNum").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t bitrate = Nan::Get(params, Nan::New("bitrate").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  encoder_ = ttLibC_AcEncoder_make(
    sampleRate,
    channelNum,
    bitrate,
    frameType);
#endif
}

AudioConverterEncoder::~AudioConverterEncoder() {
#ifdef __ENABLE_APPLE__
  ttLibC_AcEncoder_close(&encoder_);
#endif
  strongCallback_.Reset();
}

bool AudioConverterEncoder::asyncCallback(ttLibC_Frame *frame) {
  Local<Value> call = Nan::New(strongCallback_);
  Nan::Callback callback(call.As<Function>());
  Local<Object> jsFrame = Nan::New(jsFrame_);
  Frame::setFrame(jsFrame, frame);
  Local<Value> args[] = {
    Nan::Null(),
    jsFrame
  };
  Local<Value> result = callback.Call(2, args);
  if(result->IsTrue()) {
    return true;
  }
  if(result->IsUndefined()) {
    puts("応答が設定されていません。");
  }
  return false;
}

bool AudioConverterEncoder::encodeCallback(void *ptr, ttLibC_Audio *audio) {
  AudioConverterEncoder *encoder = (AudioConverterEncoder *)ptr;
  Nan::AsyncQueueWorker(new FramePassingWorker(
    (ttLibC_Frame *)audio,
    encoder));
  return true;
}

bool AudioConverterEncoder::encode(ttLibC_Frame *frame) {
#ifdef __ENABLE_APPLE__
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
    return false;
  }
  if(frame->type != frameType_pcmS16) {
    puts("pcmS16のみ処理可能です。");
    return false;
  }
  strongCallback_.Reset(callback_); // 普通のcallbackをそのまま使うとLocal<Object>が蒸発するので、不可能になる。 Persistentにいれて、蒸発しないようにしておく。
  return ttLibC_AcEncoder_encode(
    encoder_,
    (ttLibC_PcmS16 *)frame,
    encodeCallback,
    this);
#else
  return false;
#endif
}

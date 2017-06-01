#include "../predef.h"
#include "msH264.h"
#include "../frame.h"
#include <ttLibC/frame/video/yuv420.h>

#include <ttLibC/allocator.h>

void TTLIBJSGYP_CDECL MSH264Encoder::classInit(Local<Object> target) {
  Local<Object> obj = Nan::New<Object>();
  Nan::Set(obj, Nan::New("listEncoders").ToLocalChecked(), Nan::GetFunction(Nan::New<FunctionTemplate>(ListEncoders)).ToLocalChecked());
  Nan::Set(target, Nan::New("MsH264").ToLocalChecked(), obj);
}

NAN_METHOD(MSH264Encoder::ListEncoders) {
#ifdef __ENABLE_WIN32__
  if(info.Length() < 1) {
    info.GetReturnValue().Set(false);
    return;
  }
  info.GetReturnValue().Set(
    ttLibC_MsH264Encoder_listEncoders(listEncoderCallback, &info[0])
  );
#endif
}

bool MSH264Encoder::listEncoderCallback(void *ptr, const char *name) {
  Local<Value> *callback_ = (Local<Value> *)ptr;
  Nan::Callback callback((*callback_).As<Function>());
  Local<Value> args[] = {
    Nan::Null(),
    Nan::New(name).ToLocalChecked()
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

MSH264Encoder::MSH264Encoder(Local<Object> params) : Encoder() {
  type_ = get_msH264;
#ifdef __ENABLE_WIN32__
  // ここで初期化を適当に実施してみようと思う。
  uint32_t width   = Nan::Get(params, Nan::New("width").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t height  = Nan::Get(params, Nan::New("height").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  uint32_t bitrate = Nan::Get(params, Nan::New("bitrate").ToLocalChecked()).ToLocalChecked()->Uint32Value();
  std::string name(*String::Utf8Value(Nan::Get(params, Nan::New("encoder").ToLocalChecked()).ToLocalChecked()->ToString()));
  // とりあえずEnumで必要なデータの取得はできたので、このまま進める。
  // こんな感じかな。該当エンコーダーをここで指定して、そこに対してencodeを実施する的な感じで
  encoder_ = ttLibC_MsH264Encoder_make(name.c_str(), width, height, bitrate);
  // 細かい設定も可能にしておきたいところだが・・・
  // あとで考えよう
  pts_ = 0;
#endif
}

MSH264Encoder::~MSH264Encoder() {
#ifdef __ENABLE_WIN32__
  ttLibC_MsH264Encoder_close(&encoder_);
#endif
}

bool MSH264Encoder::encodeCallback(void *ptr, ttLibC_H264 *h264) {
  puts("frameを取得した。");
  return true;
}

bool MSH264Encoder::encode(ttLibC_Frame *frame) {
#ifdef __ENABLE_WIN32__
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
  }
  if(frame->type != frameType_yuv420) {
    puts("yuv420のみ処理可能です。");
    return false;
  }
  return ttLibC_MsH264Encoder_encode(
    encoder_,
    (ttLibC_Yuv420 *)frame,
    encodeCallback,
    this);
#else
  return false;
#endif
}
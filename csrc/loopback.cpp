#include "predef.h"
#include "loopback.h"
#include "frame.h"

#include <string>

void CDECL Loopback::classInit(Local<Object> target) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Loopback").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  SetPrototypeMethod(tpl, "queryFrame", QueryFrame);
  constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(
    target,
    Nan::New("Loopback").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(Loopback::New) {
#ifdef __ENABLE_WIN32__
  if(info.IsConstructCall()) {
    // パラメーターがおかしかったら例外を投げたいところ
    Loopback *loopback = new Loopback(info);
    loopback->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  }
  else {
    Local<Value> *argv = new Local<Value>[info.Length()];
    for(int i = 0;i < info.Length();++ i) {
      argv[i] = info[i];
    }
    Local<Function> cons = Nan::New(constructor());
    info.GetReturnValue().Set(Nan::NewInstance(cons, (const int)info.Length(), argv).ToLocalChecked());
    delete[] argv;
  }
#else
  puts("windowsではない環境では、Loopbackは利用できません。");
#endif
}

NAN_METHOD(Loopback::ListDevice) {
#ifdef __ENABLE_WIN32__
  // とりあえず実装するわけにはいかなそうなので放置しとく。
#endif
}

NAN_METHOD(Loopback::QueryFrame) {
  bool result = false;
#ifdef __ENABLE_WIN32__
  Loopback *loopback = Nan::ObjectWrap::Unwrap<Loopback>(info.Holder());
  if(loopback != NULL) {
    loopback->callback_ = info[0];
    result = ttLibC_MmAudioLoopback_queryFrame(loopback->loopback_, captureCallback, loopback);
  }
#endif
  info.GetReturnValue().Set(result);
}

bool Loopback::captureCallback(void *ptr, ttLibC_Frame *frame) {
  Loopback *loopback = (Loopback *)ptr;
  Nan::Callback callback(loopback->callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(loopback->jsPcmFrame_);
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

Loopback::Loopback(Nan::NAN_METHOD_ARGS_TYPE info) {
  std::string locale("");
  std::string device("");
  if(info.Length() >= 1) {
    locale = std::string(*String::Utf8Value(info[0]->ToString()));
  }
  if(info.Length() == 2) {
    // 2の場合はdeviceの指定があるもんだとして動作する。
    device = std::string(*String::Utf8Value(info[1]->ToString()));
  }
  char *c_locale = NULL;
  char *c_device = NULL;
  if(locale.length() != 0) {
    c_locale = (char *)locale.c_str();
  }
  if(device.length() != 0) {
    c_device = (char *)device.c_str();
  }
  loopback_ = ttLibC_MmAudioLoopback_make(
    (const char *)c_locale,
    (const char *)c_device);
  jsPcmFrame_.Reset(Frame::newInstance());
}

Loopback::~Loopback() {
  ttLibC_MmAudioLoopback_close(&loopback_);
  jsPcmFrame_.Reset();
}

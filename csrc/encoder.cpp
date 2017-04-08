#include "encoder.h"
#include "frame.h"

#include "encoder/mp3lame.h"
#include "encoder/openh264.h"

#include <string>

void Encoder::classInit(Local<Object> target) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Encoder").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  SetPrototypeMethod(tpl, "encode",             Encode);
  SetPrototypeMethod(tpl, "forceNextFrameType", ForceNextFrameType);
  SetPrototypeMethod(tpl, "setRCMode",          SetRCMode);
  SetPrototypeMethod(tpl, "setIDRInterval",     SetIDRInterval);
  SetPrototypeMethod(tpl, "forceNextKeyFrame",  ForceNextKeyFrame);
  SetPrototypeMethod(tpl, "setBitrate",         SetBitrate);
  SetPrototypeMethod(tpl, "setComplexity",      SetComplexity);
  SetPrototypeMethod(tpl, "setCodecControl",    SetCodecControl);
  constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(
    target,
    Nan::New("Encoder").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(Encoder::New) {
  if(info.IsConstructCall()) {
    // ここでどのcodecの動作であるか判定しなければいけないな。
    std::string type(*String::Utf8Value(info[0]->ToString()));
    Encoder *encoder = NULL;
    if(type == "mp3lame") {
      encoder = new Mp3lameEncoder(info[1]->ToObject());
    }
    else if(type == "openh264") {
      encoder = new Openh264Encoder(info[1]->ToObject());
    }
    if(encoder != NULL) {
      encoder->Wrap(info.This());
    }
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
}

NAN_METHOD(Encoder::Encode) {
  if(info.Length() == 2) {
    Encoder *encoder = Nan::ObjectWrap::Unwrap<Encoder>(info.Holder());
    if(encoder == NULL) {
      puts("encoderがありません。");
      info.GetReturnValue().Set(false);
      return;
    }
    encoder->callback_ = info[1];
    info.GetReturnValue().Set(
      encoder->encode(Frame::refFrame(info[0]->ToObject()))
    );
    return;
  }
}

NAN_METHOD(Encoder::ForceNextFrameType) {

}

NAN_METHOD(Encoder::SetRCMode) {

}

NAN_METHOD(Encoder::SetIDRInterval) {

}

NAN_METHOD(Encoder::ForceNextKeyFrame) {

}

NAN_METHOD(Encoder::SetBitrate) {

}

NAN_METHOD(Encoder::SetComplexity) {

}

NAN_METHOD(Encoder::SetCodecControl) {

}

Encoder::Encoder() {
  jsFrame_.Reset(Frame::newInstance());
}
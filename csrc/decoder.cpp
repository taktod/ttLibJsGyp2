#include "decoder.h"
#include "frame.h"

#include <string.h>

void Decoder::classInit(Local<Object> target) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Writer").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  SetPrototypeMethod(tpl, "decode", Decode);
  SetPrototypeMethod(tpl, "setCodecControl", SetCodecControl);
  constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(
    target,
    Nan::New("Writer").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(Decoder::New) {
  if(info.IsConstructCall()) {
    Decoder *decoder = new Decoder(info);
    decoder->Wrap(info.This());
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

NAN_METHOD(Decoder::Decode) {

}

NAN_METHOD(Decoder::SetCodecControl) {

}

Decoder::Decoder(Nan::NAN_METHOD_ARGS_TYPE info) {
  // type指定
}

Decoder::~Decoder() {

}
#include "resampler.h"
#include "frame.h"

#include "resampler/audio.h"

#include <string>

void Resampler::classInit(Local<Object> target) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Resampler").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  SetPrototypeMethod(tpl, "resample",          Resample);
  SetPrototypeMethod(tpl, "setRate",           SetRate);
  SetPrototypeMethod(tpl, "setTempo",          SetTempo);
  SetPrototypeMethod(tpl, "setRateChange",     SetRateChange);
  SetPrototypeMethod(tpl, "setTempoChange",    SetTempoChange);
  SetPrototypeMethod(tpl, "setPitch",          SetPitch);
  SetPrototypeMethod(tpl, "setPitchOctaves",   SetPitchOctaves);
  SetPrototypeMethod(tpl, "setPitchSemiTones", SetPitchSemiTones);
  constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(
    target,
    Nan::New("Resampler").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(Resampler::New) {
  if(info.IsConstructCall()) {
    // ここでどのcodecの動作であるか判定しなければいけないな。
    std::string type(*String::Utf8Value(info[0]->ToString()));
    Resampler *resampler = NULL;
    if(type == "audio") {
      resampler = new AudioResampler(info[1]->ToObject());
    }
    if(resampler != NULL) {
      resampler->Wrap(info.This());
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

NAN_METHOD(Resampler::Resample) {
  if(info.Length() == 2) {
    Resampler *resampler = Nan::ObjectWrap::Unwrap<Resampler>(info.Holder());
    if(resampler == NULL) {
      puts("resamplerがありません、");
      info.GetReturnValue().Set(false);
      return;
    }
    resampler->callback_ = info[1];
    info.GetReturnValue().Set(
      resampler->resample(Frame::refFrame(info[0]->ToObject()))
    );
    return;
  }
  info.GetReturnValue().Set(false);
}

NAN_METHOD(Resampler::SetRate) {

}

NAN_METHOD(Resampler::SetTempo) {

}

NAN_METHOD(Resampler::SetRateChange) {

}

NAN_METHOD(Resampler::SetTempoChange) {

}

NAN_METHOD(Resampler::SetPitch) {

}

NAN_METHOD(Resampler::SetPitchOctaves) {

}

NAN_METHOD(Resampler::SetPitchSemiTones) {

}

Resampler::Resampler() {
  jsFrame_.Reset(Frame::newInstance());
}
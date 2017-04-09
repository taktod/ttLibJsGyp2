#include "decoder.h"
#include "frame.h"

#include "decoder/avcodec.h"

#include <string>

class DummyDecoder : public Decoder {
public:
  DummyDecoder() : Decoder() {}
  bool decode(ttLibC_Frame *frame) {
    return false;
  }
private:
  ~DummyDecoder() {}
};

void Decoder::classInit(Local<Object> target) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Decoder").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  SetPrototypeMethod(tpl, "decode",          Decode);
  SetPrototypeMethod(tpl, "setCodecControl", SetCodecControl);
  constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(
    target,
    Nan::New("Decoder").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(Decoder::New) {
  if(info.IsConstructCall()) {
    // ここでどのcodecの動作であるか判定しなければいけないな。
    std::string type(*String::Utf8Value(info[0]->ToString()));
    Decoder *decoder = NULL;
    if(type == "avcodec") {
      decoder = new AvcodecDecoder(info[1]->ToObject());
    }
    else {
      decoder = new DummyDecoder();
    }
    if(decoder != NULL) {
      decoder->Wrap(info.This());
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

NAN_METHOD(Decoder::Decode) {
  if(info.Length() == 2) {
    Decoder *decoder = Nan::ObjectWrap::Unwrap<Decoder>(info.Holder());
    if(decoder == NULL) {
      puts("decoderがありません。");
      info.GetReturnValue().Set(false);
      return;
    }
    decoder->callback_ = info[1];
    info.GetReturnValue().Set(
      decoder->decode(Frame::refFrame(info[0]->ToObject()))
    );
    return;
  }
  info.GetReturnValue().Set(false);
}

NAN_METHOD(Decoder::SetCodecControl) {

}

Decoder::Decoder() {
  jsFrame_.Reset(Frame::newInstance());
}
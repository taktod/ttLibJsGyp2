#include "decoder.h"
#include "frame.h"

#include "decoder/avcodec.h"

#include <string.h>

void Decoder::classInit(Local<Object> target) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Decoder").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  SetPrototypeMethod(tpl, "decode", Decode);
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
    String::Utf8Value type(info[0]->ToString());
    int typeLength = strlen(*type);
    Decoder *decoder = NULL;
    switch(typeLength) {
    case 4:
      if(strcmp(*type, "jpeg") == 0) {
      }
      else if(strcmp(*type, "opus") == 0) {
      }
      break;
    case 5:
      if(strcmp(*type, "speex") == 0) {
      }
      break;
    case 6:
      if(strcmp(*type, "theora") == 0) {
      }
      else if(strcmp(*type, "vorbis") == 0) {
      }
      break;
    case 7:
      if(strcmp(*type, "avcodec") == 0) {
        decoder = new AvcodecDecoder(info[1]->ToObject());
      }
      else if(strcmp(*type, "mp3lame") == 0) {
      }
      break;
    default:
      break;
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
    Local<Value> jsFrame = info[0];
    Local<Value> callback = info[1];
    Decoder *decoder = Nan::ObjectWrap::Unwrap<Decoder>(info.Holder());
    if(decoder == NULL) {
      puts("decoderがありません。");
      info.GetReturnValue().Set(false);
      return;
    }
    decoder->callback_ = callback;
    info.GetReturnValue().Set(decoder->decode(Frame::refFrame(jsFrame->ToObject())));
    return;
  }
  info.GetReturnValue().Set(false);
}

NAN_METHOD(Decoder::SetCodecControl) {

}

Decoder::Decoder() {
  decoder_ = NULL;
  jsFrame_.Reset(Frame::newInstance());
}
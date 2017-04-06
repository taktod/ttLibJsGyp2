#include "encoder.h"
#include "frame.h"

#include "encoder/openh264.h"

#include <string.h>

void Encoder::classInit(Local<Object> target) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Encoder").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  SetPrototypeMethod(tpl, "encode", Encode);
  SetPrototypeMethod(tpl, "setCodecControl", SetCodecControl);
  constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(
    target,
    Nan::New("Encoder").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(Encoder::New) {
  if(info.IsConstructCall()) {
    // ここでどのcodecの動作であるか判定しなければいけないな。
    String::Utf8Value type(info[0]->ToString());
    int typeLength = strlen(*type);
    Encoder *encoder = NULL;
    switch(typeLength) {
    case 4:
      if(strcmp(*type, "faac") == 0) {
      }
      else if(strcmp(*type, "jpeg") == 0) {
      }
      else if(strcmp(*type, "opus") == 0) {
      }
      else if(strcmp(*type, "x264") == 0) {
      }
      else if(strcmp(*type, "x265") == 0) {
      }
      break;
    case 5:
      if(strcmp(*type, "speex") == 0) {
      }
      break;
    case 6:
      if(strcmp(*type, "fdkaac") == 0) {
      }
      else if(strcmp(*type, "theora") == 0) {
      }
      else if(strcmp(*type, "vorbis") == 0) {
      }
      break;
    case 7:
      if(strcmp(*type, "avcodec") == 0) {
      }
      else if(strcmp(*type, "mp3lame") == 0) {
      }
      break;
    case 8:
      if(strcmp(*type, "openh264") == 0) {
        encoder = new Openh264Encoder(info[1]->ToObject());
      }
      break;
    default:
      break;
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
  // ここつくってないからこうなるのか・・・
  if(info.Length() == 2) {
    Local<Value> jsFrame = info[0];
    Local<Value> callback = info[1];
    Encoder *encoder = Nan::ObjectWrap::Unwrap<Encoder>(info.Holder());
    if(encoder == NULL) {
      puts("encoderがありません。");
      info.GetReturnValue().Set(false);
      return;
    }
    encoder->callback_ = callback;
    info.GetReturnValue().Set(
      encoder->encode(Frame::refFrame(jsFrame->ToObject()))
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
  encoder_ = NULL;
  jsFrame_.Reset(Frame::newInstance());
}
#include "avcodec.h"
#include "../frame.h"

AvcodecDecoder::AvcodecDecoder(Local<Object> params) : Decoder() {
  type_ = gdt_avcodec;
  Local<Value> typeValue = Nan::Get(params, Nan::New("type").ToLocalChecked()).ToLocalChecked();
  String::Utf8Value type(typeValue->ToString());
  ttLibC_Frame_Type frameType = Frame::getFrameType(*type);
  switch(frameType) {
  case frameType_h264:
    {
      Local<Value> width  = Nan::Get(params, Nan::New("width").ToLocalChecked()).ToLocalChecked();
      Local<Value> height = Nan::Get(params, Nan::New("height").ToLocalChecked()).ToLocalChecked();
      decoder_ = ttLibC_AvcodecVideoDecoder_make(frameType, width->Uint32Value(), height->Uint32Value());
    }
    break;
  case frameType_aac:
    {
      Local<Value> sampleRate = Nan::Get(params, Nan::New("sampleRate").ToLocalChecked()).ToLocalChecked();
      Local<Value> channelNum = Nan::Get(params, Nan::New("channelNum").ToLocalChecked()).ToLocalChecked();
      decoder_ = ttLibC_AvcodecAudioDecoder_make(frameType, sampleRate->Uint32Value(), channelNum->Uint32Value());
    }
    break;
  default:
    break;
  }
}

AvcodecDecoder::~AvcodecDecoder() {
  ttLibC_AvcodecDecoder_close(&decoder_);
}

bool AvcodecDecoder::decodeCallback(void *ptr, ttLibC_Frame *ttFrame) {
  AvcodecDecoder *decoder = (AvcodecDecoder *)ptr;
  auto callback = new Nan::Callback(decoder->callback_.As<Function>());
  Local<Object> jsFrame = Nan::New(decoder->jsFrame_);
  Frame::setFrame(jsFrame, ttFrame);
  Local<Value> args[] = {
    Nan::Null(),
    jsFrame
  };
  Local<Value> result = callback->Call(2, args);
  if(result->IsTrue()) {
    return true;
  }
  if(result->IsUndefined()) {
    puts("応答が設定されていません。");
  }
  return false;
}

bool AvcodecDecoder::decode(ttLibC_Frame *frame) {
  return ttLibC_AvcodecDecoder_decode(
    decoder_,
    frame,
    decodeCallback,
    this);
}

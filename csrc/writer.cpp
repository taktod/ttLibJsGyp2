#include "writer.h"
#include "frame.h"

#include <string.h>

void Writer::classInit(Local<Object> target) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Writer").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  SetPrototypeMethod(tpl, "writeFrame", WriteFrame);
  SetPrototypeMethod(tpl, "writeInfo", WriteInfo);
  SetPrototypeMethod(tpl, "setMode", SetMode);
  constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(
    target,
    Nan::New("Writer").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

NAN_METHOD(Writer::New) {
  if(info.IsConstructCall()) {
    // パラメーターがおかしかったら例外を投げたいところ
    String::Utf8Value type(info[0]->ToString());
    Writer *writer = new Writer(info);
    writer->Wrap(info.This());
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

NAN_METHOD(Writer::WriteFrame) {
  if(info.Length() == 2) {
    Local<Value> jsFrame = info[0];
    Local<Value> callback = info[1];
    Writer *writer = Nan::ObjectWrap::Unwrap<Writer>(info.Holder());
    if(writer == NULL || writer->writer_ == NULL) {
      puts("writerがありません。");
      info.GetReturnValue().Set(false);
      return;
    }
    writer->callback_ = callback;
    switch(writer->writer_->type) {
    case containerType_flv:
      if(ttLibC_FlvWriter_write(
          (ttLibC_FlvWriter *)writer->writer_,
          Frame::refFrame(jsFrame->ToObject()),
          writeCallback,
          writer)) {
        info.GetReturnValue().Set(true);
        return;
      }
      break;
    case containerType_mkv:
    case containerType_webm:
      if(ttLibC_MkvWriter_write(
          writer->writer_,
          Frame::refFrame(jsFrame->ToObject()),
          writeCallback,
          writer)) {
        info.GetReturnValue().Set(true);
        return;
      }
      break;
    case containerType_mp4:
      if(ttLibC_Mp4Writer_write(
          writer->writer_,
          Frame::refFrame(jsFrame->ToObject()),
          writeCallback,
          writer)) {
        info.GetReturnValue().Set(true);
        return;
      }
      break;
    case containerType_mpegts:
      if(ttLibC_MpegtsWriter_write(
          writer->writer_,
          Frame::refFrame(jsFrame->ToObject()),
          writeCallback,
          writer)) {
        info.GetReturnValue().Set(true);
        return;
      }
      break;
    default:
      puts("不明なコンテナでした");
      break;
    }
  }
  info.GetReturnValue().Set(false);
}

NAN_METHOD(Writer::WriteInfo) {
  if(info.Length() == 1) {
    Local<Value> callback = info[0];
    Writer *writer = Nan::ObjectWrap::Unwrap<Writer>(info.Holder());
    if(writer == NULL || writer->writer_ == NULL) {
      puts("writerがありません。");
      info.GetReturnValue().Set(false);
      return;
    }
    writer->callback_ = callback;
    switch(writer->writer_->type) {
    case containerType_mpegts:
      if(ttLibC_MpegtsWriter_writeInfo(
          writer->writer_,
          writeCallback,
          writer)) {
        info.GetReturnValue().Set(true);
      }
      break;
    default:
      info.GetReturnValue().Set(true);
      return;
    }
  }
  info.GetReturnValue().Set(false);
}
NAN_METHOD(Writer::SetMode) {
  if(info.Length() == 1) {
    Writer *writer = Nan::ObjectWrap::Unwrap<Writer>(info.Holder());
    if(writer == NULL || writer->writer_ == NULL) {
      puts("writerがありません。");
      info.GetReturnValue().Set(false);
      return;
    }
    writer->writer_->mode = info[1]->Uint32Value();
  }
  info.GetReturnValue().Set(false);
}

Writer::Writer(Nan::NAN_METHOD_ARGS_TYPE info) {
  // 基本このデータはtype length codec...となっている。
  // flvだけ例外でtype videoCodec audioCodecとなっている。
  String::Utf8Value type(info[0]->ToString());
  if(strcmp((const char *)*type, "flv") == 0) {
    String::Utf8Value videoCodec(info[1]->ToString());
    String::Utf8Value audioCodec(info[2]->ToString());
    writer_ = (ttLibC_ContainerWriter *)ttLibC_FlvWriter_make(
        Frame::getFrameType(*videoCodec),
        Frame::getFrameType(*audioCodec));
  }
  else {
    int num = info.Length() - 2;
    ttLibC_Frame_Type *types = new ttLibC_Frame_Type[num];
    for(int i = 0;i < num;++ i) {
      String::Utf8Value codec(info[i + 2]->ToString());
      types[i] = Frame::getFrameType(*codec);
    }
    if(strcmp((const char *)*type, "mkv") == 0) {
      writer_ = ttLibC_MkvWriter_make_ex(types, num, info[1]->Uint32Value());
    }
    else if(strcmp((const char *)*type, "mp4") == 0) {
      writer_ = ttLibC_Mp4Writer_make_ex(types, num, info[1]->Uint32Value());
    }
    else if(strcmp((const char *)*type, "webm") == 0) {
      writer_ = ttLibC_MkvWriter_make_ex(types, num, info[1]->Uint32Value());
      writer_->type = containerType_webm;
    }
    else if(strcmp((const char *)*type, "mpegts") == 0) {
      writer_ = ttLibC_MpegtsWriter_make_ex(types, num, info[1]->Uint32Value());
    }
    else {
      writer_ = NULL;
    }
    delete[] types;
  }
}

Writer::~Writer() {
  ttLibC_ContainerWriter_close(&writer_);
}

bool Writer::writeCallback(
    void *ptr,
    void *data,
    size_t data_size) {
  // あとはこのデータをcallbackで応答すればよい。
  Writer *writer = (Writer *)ptr;
  auto callback = new Nan::Callback(writer->callback_.As<Function>());
  Local<Object> binary = Nan::CopyBuffer((char *)data, data_size).ToLocalChecked();
  Local<Value> args[] = {
    Nan::Null(),
    binary
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
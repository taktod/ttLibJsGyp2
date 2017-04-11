#include "netConnection.h"
#include "netStream.h"
#include "amf0Helper.h"
#include "../frame.h"

void NetStream::classInit(Local<Object> target) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("NetStream").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  SetPrototypeMethod(tpl, "play", Play);
  SetPrototypeMethod(tpl, "setBufferLength", SetBufferLength);
  SetPrototypeMethod(tpl, "setFrameListener", SetFrameListener);
  SetPrototypeMethod(tpl, "publish", Publish);
  SetPrototypeMethod(tpl, "queueFrame", QueueFrame);
  SetPrototypeMethod(tpl, "addEventListener", AddEventListener);

  constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());

  Nan::Set(
    target,
    Nan::New("NetStream").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

void NetStream::doUpdate() {
}

void NetStream::doCallback(rtmpEventList_t *event) {
  if(!isCalled_) {
    return;
  }
  std::string name(event->name);
  if(name == "onStatusEvent") {
    if(callObjectList_->size == 0) {
      return;
    }
    while(true) {
      ttLibC_Amf0Object *obj = (ttLibC_Amf0Object *)ttLibC_StlList_refFirst(callObjectList_);
      if(obj == NULL) {
        break;
      }
      ttLibC_StlList_remove(callObjectList_, obj);
      Local<Object> jsObject = Nan::New<Object>();
      Nan::Set(jsObject, Nan::New("info").ToLocalChecked(), makeJsObject_from_ttLibCAmf0(obj));
      ttLibC_Amf0_close(&obj);
      Local<Value> args[] = {jsObject};
      event->callback->Call(1, args);
    }
    isCalled_ = false;
  }
  else if(name == "onFrame") {
    if(receiveFrameList_->size == 0) {
      return;
    }
    while(true) {
      ttLibC_Frame *frame = (ttLibC_Frame *)ttLibC_StlList_refFirst(receiveFrameList_);
      if(frame == NULL) {
        break;
      }
      ttLibC_StlList_remove(receiveFrameList_, frame);
      // ここでframeのcallbackが必要になる。
      Local<Object> jsFrame;
      if(ttLibC_Frame_isAudio(frame)) {
        jsFrame = Nan::New(jsAudioFrame_);
      }
      else if(ttLibC_Frame_isVideo(frame)) {
        jsFrame = Nan::New(jsVideoFrame_);
      }
      Frame::setFrame(jsFrame, frame);
      Local<Value> args[] = {
        Nan::Null(),
        jsFrame
      };
      event->callback->Call(2, args);
      ttLibC_Frame_close(&frame);
    }
    isCalled_ = false;
  }
}

NAN_METHOD(NetStream::New) {
  if(info.IsConstructCall()) {
    NetConnection *conn = Nan::ObjectWrap::Unwrap<NetConnection>(info[0]->ToObject());
    NetStream *stream = conn->newStream();
    stream->Wrap(info.This());
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

NAN_METHOD(NetStream::Play) {
  if(info.Length() < 1) {
    puts("パラメーターは1つ以上");
    info.GetReturnValue().Set(false);
    return;
  }
  if(!info[0]->IsString()) {
    puts("１つ目は文字列であるべき");
    info.GetReturnValue().Set(false);
    return;
  }
  bool hasVideo = true;
  bool hasAudio = true;
  if(info.Length() > 1) {
    if(info[1]->IsBoolean()) {
      hasVideo = info[1]->IsTrue();
    }
  }
  if(info.Length() > 2) {
    if(info[2]->IsBoolean()) {
      hasAudio = info[2]->IsTrue();
    }
  }
  NetStream* stream = Nan::ObjectWrap::Unwrap<NetStream>(info.Holder());
  ttLibC_RtmpStream_play(stream->stream_, (const char *)*String::Utf8Value(info[0]->ToString()), hasVideo, hasAudio);
  info.GetReturnValue().Set(true);
}

NAN_METHOD(NetStream::SetBufferLength) {
  if(info.Length() != 1) {
    puts("パラメーターは1つ");
    info.GetReturnValue().Set(false);
    return;
  }
  if(!info[0]->IsNumber()) {
    puts("パラメーターは数値であるべき");
    info.GetReturnValue().Set(false);
    return;
  }
  NetStream* stream = Nan::ObjectWrap::Unwrap<NetStream>(info.Holder());
  int value = (int)info[0]->NumberValue();
  ttLibC_RtmpStream_setBufferLength(stream->stream_, value * 1000);
  info.GetReturnValue().Set(true);
}

NAN_METHOD(NetStream::SetFrameListener) {
  NetStream *stream = Nan::ObjectWrap::Unwrap<NetStream>(info.Holder());
  stream->eventManager_->addEventListener(stream, info, type_frame);
}

NAN_METHOD(NetStream::AddEventListener) {
  NetStream *stream = Nan::ObjectWrap::Unwrap<NetStream>(info.Holder());
  stream->eventManager_->addEventListener(stream, info, type_event);
}

NAN_METHOD(NetStream::Publish) {
  if(info.Length() != 1) {
    puts("パラメーターは1つ");
    info.GetReturnValue().Set(false);
    return;
  }
  if(!info[0]->IsString()) {
    puts("パラメーターは文字列であるべき");
    info.GetReturnValue().Set(false);
    return;
  }
  NetStream* stream = Nan::ObjectWrap::Unwrap<NetStream>(info.Holder());
  // publishを実行する。
  ttLibC_RtmpStream_publish(stream->stream_, (const char *)(*String::Utf8Value(info[0]->ToString())));
  info.GetReturnValue().Set(true);
}

NAN_METHOD(NetStream::QueueFrame) {
  if(info.Length() != 1) {
    puts("パラメーターはフレーム1つである必要があります。");
    info.GetReturnValue().Set(false);
    return;
  }
  if(!info[0]->IsObject()) {
    puts("1st argはframeオブジェクトでないとだめです。");
    info.GetReturnValue().Set(false);
    return;
  }
  NetStream* stream = Nan::ObjectWrap::Unwrap<NetStream>(info.Holder());
  if(!ttLibC_RtmpStream_addFrame(
      stream->stream_,
      Frame::refFrame(info[0]->ToObject()))) {
    info.GetReturnValue().Set(false);
  }
/*
  ttLibC_Frame *frame = stream->frameManager_->getFrame(info[0]->ToObject());
  if(frame == NULL) {
    puts("frameを復元できなかった。");
    info.GetReturnValue().Set(Nan::New(false));
    return;
  }
  if(!ttLibC_RtmpStream_addFrame(
      stream->stream_,
      frame)) {
    info.GetReturnValue().Set(Nan::New(false));
    return;
  }*/
  info.GetReturnValue().Set(Nan::New(true));
}

NetStream::NetStream(
    ttLibC_RtmpConnection *conn,
    EventManager *eventManager) : eventManager_(eventManager) {
  stream_ = ttLibC_RtmpStream_make(conn);
  ttLibC_RtmpStream_addEventListener(stream_, onStatusEventCallback, this);
  ttLibC_RtmpStream_addFrameListener(stream_, onFrameCallback, this);
  isCalled_ = false;
  callObjectList_ = ttLibC_StlList_make();
  receiveFrameList_ = ttLibC_StlList_make();
  jsVideoFrame_.Reset(Frame::newInstance());
  jsAudioFrame_.Reset(Frame::newInstance());
}

static bool clearObjectListCallback(void *ptr, void *item) {
  ttLibC_Amf0Object *amf0_obj = (ttLibC_Amf0Object *)item;
  ttLibC_Amf0_close(&amf0_obj);
  return true;
}

static bool clearFrameListCallback(void *ptr, void *item) {
  ttLibC_Frame *frame = (ttLibC_Frame *)item;
  ttLibC_Frame_close(&frame);
  return true;
}

NetStream::~NetStream() {
  ttLibC_StlList_forEach(callObjectList_, clearObjectListCallback, NULL);
  ttLibC_StlList_close(&callObjectList_);
  ttLibC_StlList_forEach(receiveFrameList_, clearFrameListCallback, NULL);
  ttLibC_StlList_close(&receiveFrameList_);
  ttLibC_RtmpStream_close(&stream_);
  jsVideoFrame_.Reset();
  jsAudioFrame_.Reset();
}

bool NetStream::onStatusEventCallback(void *ptr, ttLibC_Amf0Object *obj) {
  NetStream *stream = (NetStream *)ptr;
  stream->isCalled_ = true;
  stream->eventManager_->isCalled_ = true;
  ttLibC_StlList_addLast(stream->callObjectList_, ttLibC_Amf0_clone(obj));
  return true;
}

bool NetStream::onFrameCallback(void *ptr, ttLibC_Frame *frame) {
  NetStream *stream = (NetStream *)ptr;
  stream->isCalled_ = true;
  stream->eventManager_->isCalled_ = true;
  ttLibC_StlList_addLast(stream->receiveFrameList_, ttLibC_Frame_clone(NULL, frame));
  return true;
}
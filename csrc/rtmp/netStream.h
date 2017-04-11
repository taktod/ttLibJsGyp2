#ifndef CSRC_RTMP_NETSTREAM_H
#define CSRC_RTMP_NETSTREAM_H

#include "eventManager.h"
#include <ttLibC/net/client/rtmp.h>
#include <ttLibC/util/stlListUtil.h>
#include <ttLibC/util/amfUtil.h>

class NetStream : public RtmpClientBase {
public:
  static void classInit(Local<Object> target);
  void doUpdate();
  void doCallback(rtmpEventList_t *event);
  NetStream(ttLibC_RtmpConnection *conn, EventManager *eventManager);
private:
  static NAN_METHOD(New);
  static NAN_METHOD(Play);
  static NAN_METHOD(SetBufferLength);
  static NAN_METHOD(SetFrameListener);
  static NAN_METHOD(AddEventListener);
  // 以下はまだテストしてない。
  static NAN_METHOD(Publish);
  static NAN_METHOD(QueueFrame);

  static inline Nan::Persistent<Function> & constructor() {
    static Nan::Persistent<Function> my_constructor;
    return my_constructor;
  }
  ~NetStream();
  // 内部で利用するcallbackの受け取り動作
  static bool onStatusEventCallback(void *ptr, ttLibC_Amf0Object *obj);
  static bool onFrameCallback(void *ptr, ttLibC_Frame *frame);
  ttLibC_RtmpStream *stream_;
  EventManager *eventManager_;
  ttLibC_StlList *callObjectList_;
  ttLibC_StlList *receiveFrameList_;
  Nan::Persistent<Object> jsVideoFrame_;
  Nan::Persistent<Object> jsAudioFrame_;
};

#endif
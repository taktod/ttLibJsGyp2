#ifndef CSRC_RTMP_NETCONNECTION_H
#define CSRC_RTMP_NETCONNECTION_H

#include "eventManager.h"
#include "netStream.h"
#include <ttLibC/net/client/rtmp.h>
#include <ttLibC/util/amfUtil.h>

class NetConnection : public RtmpClientBase {
public:
  static void classInit(Local<Object> target);
  void doUpdate();
  void doCallback(rtmpEventList_t *event);

  NetStream *newStream() {
    return new NetStream(conn_, eventManager_);
  }
private:
  static NAN_METHOD(New);
  static NAN_METHOD(Connect);
  static NAN_METHOD(AddEventListener);

  static inline Nan::Persistent<Function> & constructor() {
    static Nan::Persistent<Function> my_constructor;
    return my_constructor;
  }
  NetConnection();
  ~NetConnection();
  static bool onStatusEventCallback(void *ptr, ttLibC_Amf0Object *obj);
  ttLibC_RtmpConnection *conn_;
  ttLibC_Amf0Object *callObject_;
  EventManager *eventManager_;
};

#endif
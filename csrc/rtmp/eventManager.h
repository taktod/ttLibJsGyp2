#ifndef CSRC_RTMP_EVENTMANAGER_H
#define CSRC_RTMP_EVENTMANAGER_H

#include "rtmpClientBase.h"
#include <ttLibC/util/stlListUtil.h>

typedef struct rtmpEventList_t {
  RtmpClientBase *baseObject;
  char name[256];
  long callbackId;
  Nan::Callback *callback;
} rtmpEventList_t;

typedef enum EventListenerType_e {
  type_frame,
  type_event
} EventListenerType_e;

class EventManager {
public:
  EventManager(RtmpClientBase *base);
  ~EventManager();
  void doUpdate();
  void doCallback();
  void addEventListener(
    RtmpClientBase *baseObject,
    Nan::NAN_METHOD_ARGS_TYPE info,
    EventListenerType_e type);
  bool isCalled_;
  bool hasError_;
private:
  ttLibC_StlList *eventList_;
  RtmpClientBase *base_;
};

#endif
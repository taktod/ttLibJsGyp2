#ifndef CSRC_RTMP_RTMPCLIENTBASE_H
#define CSRC_RTMP_RTMPCLIENTBASE_H

#include <nan.h>

using namespace v8;

typedef struct rtmpEventList_t rtmpEventList_t;

class RtmpClientBase : public Nan::ObjectWrap {
public:
  virtual void doUpdate() = 0;
  virtual void doCallback(rtmpEventList_t *event);
  bool isCalled_;
};

#endif
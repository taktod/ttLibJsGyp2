﻿#ifndef CSRC_LOOPBACK_H
#define CSRC_LOOPBACK_H

#include <nan.h>
#include "ttLibC/util/mmAudioLoopbackUtil.h"

using namespace v8;

class Loopback : public Nan::ObjectWrap {
public:
  static void classInit(Local<Object> target);
private:
  static NAN_METHOD(New);
  static NAN_METHOD(ListDevice);
  static NAN_METHOD(QueryFrame);
  static inline Nan::Persistent<Function> & constructor() {
    static Nan::Persistent<Function> my_constructor;
    return my_constructor;
  }
  static bool captureCallback(void *ptr, ttLibC_Frame *frame);
  Loopback(Nan::NAN_METHOD_ARGS_TYPE info);
  ~Loopback();

  ttLibC_MmAudioLoopback *loopback_;
  Nan::Persistent<Object> jsPcmFrame_;
  Local<Value>            callback_;
};

#endif

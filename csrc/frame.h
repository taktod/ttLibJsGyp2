#ifndef CSRC_FRAME_H
#define CSRC_FRAME_H

#include <nan.h>
#include <ttLibC/frame/frame.h>

using namespace v8;

class Frame : public Nan::ObjectWrap {
public:
  static void classInit(Local<Object> target);
  static Local<Object> newInstance();
  static bool setFrame(Local<Object> jsFrame, ttLibC_Frame *frame);
  static ttLibC_Frame_Type getFrameType(const char *name);
  static ttLibC_Frame *refFrame(Local<Object> jsFrame);
private:
  static NAN_METHOD(New);
  static NAN_METHOD(GetBinaryBuffer);

  static inline Nan::Persistent<Function> & constructor() {
    static Nan::Persistent<Function> my_constructor;
    return my_constructor;
  }
  explicit Frame();
  ~Frame();

  ttLibC_Frame *frame_;
  bool isRef_;
};

#endif
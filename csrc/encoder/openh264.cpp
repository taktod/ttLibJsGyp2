#include "openh264.h"
#include "../frame.h"
#include <ttLibC/encoder/openh264Encoder.h>

Openh264Encoder::Openh264Encoder(Local<Object> params) : Encoder() {
  type_ = get_openh264;
  // width height params spatialParamsArrayが必要
}

Openh264Encoder::~Openh264Encoder() {
  ttLibC_Openh264Encoder_close((ttLibC_Openh264Encoder **)&encoder_);
}

bool Openh264Encoder::encodeCallback(void *ptr, ttLibC_H264 *h264) {
  puts("encodeできました。");
  return true;
}

bool Openh264Encoder::encode(ttLibC_Frame *frame) {
  if(encoder_ == NULL) {
    puts("encoderが準備されていません。");
    return false;
  }
  if(frame->type != frameType_yuv420) {
    puts("yuv420のみ処理可能です。");
    return false;
  }
  return ttLibC_Openh264Encoder_encode(
    (ttLibC_Openh264Encoder *)encoder_,
    (ttLibC_Yuv420 *)frame,
    encodeCallback,
    this);
}
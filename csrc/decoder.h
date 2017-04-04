#ifndef CSRC_DECODER_H
#define CSRC_DECODER_H

#include <nan.h>

using namespace v8;

typedef enum gypDecoderType_e {
  gdt_audioConverter,
  gdt_avcodec,
  gdt_jpeg,
  gdt_mp3lame,
  gdt_openh264,
  gdt_opus,
  gdt_speex,
  gdt_theora,
  gdt_vorbis,
  gdt_vtDecompressSession
} gypDecoderType_e;

typedef struct gypDecoder_t {
  void *decoder;
  gypDecoderType_e type;
//  ttLibC_Frame *frame; // これ必要かな？
//  frame_t inputFrame; // これも必要かね？
} gypDecoder_t;

class Decoder : public Nan::ObjectWrap {
public:
  static void classInit(Local<Object> target);
private:
  static NAN_METHOD(New);
  static NAN_METHOD(Decode);
  static NAN_METHOD(SetCodecControl);

  static inline Nan::Persistent<Function> & constructor() {
    static Nan::Persistent<Function> my_constructor;
    return my_constructor;
  }
  explicit Decoder(Nan::NAN_METHOD_ARGS_TYPE info);
  ~Decoder();
  gypDecoder_t decoder_; // これでいいかな
};

#endif

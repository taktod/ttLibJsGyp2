#ifndef CSRC_DECODER_AVCODEC_H
#define CSRC_DECODER_AVCODEC_H

#include "../decoder.h"

class AvcodecDecoder : public Decoder {
public:
  AvcodecDecoder(Local<Object> params);
  bool decode(ttLibC_Frame *frame);
private:
  static bool decodeCallback(void *ptr, ttLibC_Frame *frame);
  ~AvcodecDecoder();
};

#endif

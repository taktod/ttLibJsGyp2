#ifndef CSRC_ENCODER_VTCOMPRESSSESSION_H
#define CSRC_ENCODER_VTCOMPRESSSESSION_H

#include "../encoder.h"
#include <ttLibC/frame/video/video.h>
#ifdef __ENABLE_APPLE__
# include <ttLibC/encoder/vtCompressSessionEncoder.h>
#endif
#include <ttLibC/util/stlListUtil.h>
#include <pthread.h>

class VtCompressSessionEncoder : public Encoder {
public:
  VtCompressSessionEncoder(Local<Object> params);
  bool encode(ttLibC_Frame *frame);
//  bool asyncCallback(ttLibC_Frame *frame);
private:
  static bool encodeCallback(void *ptr, ttLibC_Video *frame);
  ~VtCompressSessionEncoder();
#ifdef __ENABLE_APPLE__
  ttLibC_VtEncoder *encoder_;
#endif
//  Nan::Persistent<Value> strongCallback_;
  ttLibC_StlList *frameStack_;
  pthread_mutex_t frameMutex_;
};

#endif
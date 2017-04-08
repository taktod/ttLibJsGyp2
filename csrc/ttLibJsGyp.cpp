#include <nan.h>

using namespace v8;

#include "reader.h"
#include "writer.h"
#include "decoder.h"
#include "encoder.h"
#include "resampler.h"
#include "frame.h"

static NAN_MODULE_INIT(Init) {
  Reader::classInit(target);
  Writer::classInit(target);
  Decoder::classInit(target);
  Encoder::classInit(target);
  Resampler::classInit(target);
  Frame::classInit(target);
}

NODE_MODULE(ttLibJsGyp, Init);

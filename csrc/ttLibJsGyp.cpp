#include <nan.h>

using namespace v8;

#include "reader.h"
#include "writer.h"
#include "decoder.h"
#include "frame.h"

static NAN_MODULE_INIT(Init) {
  Reader::classInit(target);
  Writer::classInit(target);
  Decoder::classInit(target);
  Frame::classInit(target);
}

NODE_MODULE(ttLibJsGyp, Init);

#include <nan.h>

using namespace v8;

#include "reader.h"
#include "writer.h"
#include "frame.h"

static NAN_MODULE_INIT(Init) {
  Reader::classInit(target);
  Writer::classInit(target);
  Frame::classInit(target);
}

NODE_MODULE(ttLibJsGyp, Init);

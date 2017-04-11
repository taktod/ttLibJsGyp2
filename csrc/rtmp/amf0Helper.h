#ifndef CSRC_RTMP_AMF0HELPER_H
#define CSRC_RTMP_AMF0HELPER_H

#include <nan.h>
#include <ttLibC/util/amfUtil.h>

using namespace v8;

Local<Value> makeJsObject_from_ttLibCAmf0(ttLibC_Amf0Object *src_obj);

#endif
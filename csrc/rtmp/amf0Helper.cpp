#include "amf0Helper.h"

Local<Value> makeJsObject_from_ttLibCAmf0(ttLibC_Amf0Object *src_obj) {
  switch(src_obj->type) {
  case amf0Type_Number:
    return Nan::New(*((double *)src_obj->object));
  case amf0Type_Boolean:
    return Nan::New((bool)*((uint8_t *)src_obj->object) == 1);
  case amf0Type_String:
    return Nan::New((const char *)src_obj->object).ToLocalChecked();
  case amf0Type_Object:
    {
      Local<Object> object = Nan::New<Object>();
      ttLibC_Amf0MapObject *src_lists = (ttLibC_Amf0MapObject *)src_obj->object;
      for(int i = 0;src_lists[i].key != NULL && src_lists[i].amf0_obj != NULL;++ i) {
        Nan::Set(object, Nan::New(src_lists[i].key).ToLocalChecked(), makeJsObject_from_ttLibCAmf0(src_lists[i].amf0_obj));
      }
      return object;
    }
    break;
  case amf0Type_MovieClip:
    break;
  case amf0Type_Null:
    break;
  case amf0Type_Undefined:
  case amf0Type_Reference:
    break;
  case amf0Type_Map:
    {
      Local<Object> object = Nan::New<Object>();
      ttLibC_Amf0MapObject *src_lists = (ttLibC_Amf0MapObject *)src_obj->object;
      for(int i = 0;src_lists[i].key != NULL && src_lists[i].amf0_obj != NULL;++ i) {
        Nan::Set(object, Nan::New(src_lists[i].key).ToLocalChecked(), makeJsObject_from_ttLibCAmf0(src_lists[i].amf0_obj));
      }
      return object;
    }
    break;
  case amf0Type_ObjectEnd:
  case amf0Type_Array:
  case amf0Type_Date:
  case amf0Type_LongString:
  case amf0Type_Unsupported:
  case amf0Type_RecordSet:
  case amf0Type_XmlDocument:
  case amf0Type_TypedObject:
  case amf0Type_Amf3Object:
  default:
    break;
  }
  return Nan::New("undefined").ToLocalChecked();
}

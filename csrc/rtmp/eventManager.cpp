#include "eventManager.h"
#include <ttLibC/allocator.h>

static bool doEachCallback(void *ptr, void *item) {
  if(item != NULL) {
    rtmpEventList_t *eventList = (rtmpEventList_t *)item;
    eventList->baseObject->doCallback(eventList);
  }
  return true;
}

static bool clearEventListCallback(void *ptr, void *item) {
  if(item != NULL) {
    rtmpEventList_t *eventList = (rtmpEventList_t *)item;
    if(eventList->callback) {
      delete eventList->callback;
      eventList->callback = NULL;
    }
    ttLibC_free(item);
  }
  return true;
}

EventManager::EventManager(RtmpClientBase *base) : base_(base) {
  eventList_ = ttLibC_StlList_make();
  isCalled_ = false;
  hasError_ = false;
}

EventManager::~EventManager() {
  ttLibC_StlList_forEach(eventList_, clearEventListCallback, NULL);
  ttLibC_StlList_close(&eventList_);
}

void EventManager::doUpdate() {
  base_->doUpdate();
}

void EventManager::doCallback() {
  ttLibC_StlList_forEach(eventList_, doEachCallback, NULL);
  isCalled_ = false;
}

void EventManager::addEventListener(
    RtmpClientBase *baseObject,
    Nan::NAN_METHOD_ARGS_TYPE info,
    EventListenerType_e type) {
  // まずパラメーター確認する
  rtmpEventList_t *event = NULL;
  switch(type) {
  case type_event:
    {
      if(info.Length() != 2) {
        puts("必要パラメーター数が2ではないみたいです。");
        info.GetReturnValue().Set(false);
        return;
      }
      if(!info[0]->IsString()
      || !info[1]->IsFunction()) {
        puts("引数の型が一致しません。");
        info.GetReturnValue().Set(false);
        return;
      }
      event = (rtmpEventList_t *)ttLibC_malloc(sizeof(rtmpEventList_t));
      memset(event, 0, sizeof(rtmpEventList_t));
      String::Utf8Value str(info[0]->ToString());
      strncpy(event->name, (const char *)*str, strlen((const char *)*str));
      event->callback = new Nan::Callback(info[1].As<Function>());
      event->baseObject = baseObject;
      ttLibC_StlList_addLast(eventList_, event);
    }
    break;
  case type_frame:
    {
      if(info.Length() != 1) {
        puts("必要パラメーター数が1ではないみたいです。");
        info.GetReturnValue().Set(false);
        return;
      }
      if(!info[0]->IsFunction()) {
        puts("引数の型が一致しません。");
        info.GetReturnValue().Set(false);
        return;
      }
      event = (rtmpEventList_t *)ttLibC_malloc(sizeof(rtmpEventList_t));
      strncpy(event->name, "onFrame", strlen("onFrame"));
      event->callback = new Nan::Callback(info[0].As<Function>());
      event->baseObject = baseObject;
      ttLibC_StlList_addLast(eventList_, event);
    }
    break;
  }
  info.GetReturnValue().Set(true);
}

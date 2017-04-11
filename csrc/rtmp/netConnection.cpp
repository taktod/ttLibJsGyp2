#include "netConnection.h"
#include "rtmpEventWorker.h"
#include "amf0Helper.h"

void NetConnection::classInit(Local<Object> target) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("NetConnection").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  SetPrototypeMethod(tpl, "connect", Connect);
  SetPrototypeMethod(tpl, "addEventListener", AddEventListener);
  constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());

  Nan::Set(
    target,
    Nan::New("NetConnection").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

void NetConnection::doUpdate() {
  if(!ttLibC_RtmpConnection_update(conn_, 10000)) {
    // 処理でエラーが発生したら、抜ける
    eventManager_->hasError_ = true;
    return;
  }
}

void NetConnection::doCallback(rtmpEventList_t *event) {
  if(!isCalled_) {
    // calledのフラグが立ってない場合は、今回呼ばれるべき処理ではなかった
    return;
  }
  Local<Object> jsObject = Nan::New<Object>();
  Nan::Set(jsObject, Nan::New("info").ToLocalChecked(), makeJsObject_from_ttLibCAmf0(callObject_));
  Local<Value> args[] = {jsObject};
  event->callback->Call(1, args);
  isCalled_ = false;
}

NAN_METHOD(NetConnection::New) {
  if(info.IsConstructCall()) {
    NetConnection *conn = new NetConnection();
    conn->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  }
  else {
    Local<Value> *argv = new Local<Value>[info.Length()];
    for(int i = 0;i < info.Length();++ i) {
      argv[i] = info[i];
    }
    Local<Function> cons = Nan::New(constructor());
    info.GetReturnValue().Set(Nan::NewInstance(cons, (const int)info.Length(), argv).ToLocalChecked());
    delete[] argv;
  }
}

NAN_METHOD(NetConnection::Connect) {
  if(info.Length() != 1) {
    puts("パラメーターは1つ");
    info.GetReturnValue().Set(false);
    return;
  }
  if(!info[0]->IsString()) {
    puts("パラメーターが文字列でないみたいです。");
    info.GetReturnValue().Set(false);
    return;
  }
  NetConnection *conn = Nan::ObjectWrap::Unwrap<NetConnection>(info.Holder());
  // 接続実施
  ttLibC_RtmpConnection_connect(conn->conn_, (const char *)*String::Utf8Value(info[0]->ToString()));
  // このAsyncQueueWorkerの処理はあとでちゃんとWorkerを解放してくれる模様
  Nan::AsyncQueueWorker(new RtmpEventWorker(conn->eventManager_));
}

NAN_METHOD(NetConnection::AddEventListener) {
  NetConnection *conn = Nan::ObjectWrap::Unwrap<NetConnection>(info.Holder());
  conn->eventManager_->addEventListener(conn, info, type_event);
}

NetConnection::NetConnection() {
  conn_ = ttLibC_RtmpConnection_make();
  ttLibC_RtmpConnection_addEventListener(conn_, onStatusEventCallback, this);
  isCalled_ = false;
  callObject_ = NULL;
  eventManager_ = new EventManager(this);
}

NetConnection::~NetConnection() {
  delete eventManager_;
  ttLibC_RtmpConnection_close(&conn_);
}

bool NetConnection::onStatusEventCallback(
    void *ptr,
    ttLibC_Amf0Object *obj) {
  NetConnection *conn = (NetConnection *)ptr;
  conn->isCalled_ = true;
  conn->eventManager_->isCalled_ = true;
  ttLibC_Amf0_close(&conn->callObject_);
  conn->callObject_ = ttLibC_Amf0_clone(obj);
  return true;
}
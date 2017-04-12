#include "rtmpEventWorker.h"

RtmpEventWorker::RtmpEventWorker(EventManager *eventManager) : 
  Nan::AsyncWorker(NULL), manager_(eventManager) {
}

void RtmpEventWorker::Execute() {
  manager_->doUpdate();
}

// 処理の実施は毎回おわる形になる。
// ループではなく、再度async処理を走らせることでループさせてる
void RtmpEventWorker::HandleOKCallback() {
  manager_->doCallback();
  if(!manager_->hasError_) {
    // これでちゃんと解放されてるっぽい。
    Nan::AsyncQueueWorker(new RtmpEventWorker(manager_));
  }
}

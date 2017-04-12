#ifndef CSRC_RTMP_RTMPEVENTWORKER_H
#define CSRC_RTMP_RTMPEVENTWORKER_H

#include "eventManager.h"

// 内部でsocketのやりとりを監視するループ用のworker
// 別スレッドでやらせずに、メインの動作からwhileループでcallする仕様でもよかったですね。まぁいいけど
class RtmpEventWorker : public Nan::AsyncWorker {
public:
  RtmpEventWorker(EventManager *eventManager);
  void Execute();
  void HandleOKCallback();
private:
  EventManager *manager_;
};

#endif
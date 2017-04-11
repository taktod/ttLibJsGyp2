#include "frame.h"

#include <stdlib.h>
#include <string.h>

#include <ttLibC/frame/audio/audio.h>
#include <ttLibC/frame/video/video.h>

void Frame::classInit(Local<Object> target) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Frame").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  SetPrototypeMethod(tpl, "getBinaryBuffer", GetBinaryBuffer);
  SetPrototypeMethod(tpl, "clone", Clone);

  constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(
    target,
    Nan::New("Frame").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

Local<Object> Frame::newInstance() {
  return Nan::NewInstance(
    Nan::New(constructor()),
    0,
    NULL).ToLocalChecked();
}

bool Frame::setFrame(Local<Object> jsFrame, ttLibC_Frame *ttFrame) {
  // ここでframeの内容を更新したりしないといけない。
  // 内包しているFrameオブジェクトとjsFrameの内容とttFrameの内容の３つが一致していなければならない。
  // この動作はjsFrameの内容を上書きして、内部で保持しているttLibC_Frameを更新することが目的
  /*
  type
  width
  height
  videoType key inner info

  sampleRate
  sampleNum
  channelNum

  frameType フレームごとに特殊な値
  dataPos 開始位置調整
  stride  stride調整
  */
  // jsFrameが保持しているid timebase ptsを参照しなければならない。
  // 変更している場合は、保持しているデータが更新する。
  // とりあえずはじめは全部更新でつくっておくか
  if(ttFrame->pts < 0xFFFFFFFFL) {
    Nan::Set(jsFrame, Nan::New("pts").ToLocalChecked(), Nan::New((uint32_t)ttFrame->pts));
  }
  else {
    Nan::Set(jsFrame, Nan::New("pts").ToLocalChecked(), Nan::New((double)ttFrame->pts));
  }
  Nan::Set(jsFrame, Nan::New("timebase").ToLocalChecked(), Nan::New(ttFrame->timebase));
  Nan::Set(jsFrame, Nan::New("id").ToLocalChecked(),       Nan::New(ttFrame->id));
  if(ttLibC_Frame_isAudio(ttFrame)) {
    ttLibC_Audio *audio = (ttLibC_Audio *)ttFrame;
    Nan::Set(jsFrame, Nan::New("sampleRate").ToLocalChecked(), Nan::New(audio->sample_rate));
    Nan::Set(jsFrame, Nan::New("sampleNum").ToLocalChecked(),  Nan::New(audio->sample_num));
    Nan::Set(jsFrame, Nan::New("channelNum").ToLocalChecked(), Nan::New(audio->channel_num));
    switch(ttFrame->type) {
    case frameType_aac:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("aac").ToLocalChecked());
      break;
    case frameType_adpcm_ima_wav:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("adpcmImaWav").ToLocalChecked());
      break;
    case frameType_mp3:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("mp3").ToLocalChecked());
      break;
    case frameType_nellymoser:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("nellymoser").ToLocalChecked());
      break;
    case frameType_opus:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("opus").ToLocalChecked());
      break;
    case frameType_pcm_alaw:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("pcmAlaw").ToLocalChecked());
      break;
    case frameType_pcmF32:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("pcmF32").ToLocalChecked());
      break;
    case frameType_pcm_mulaw:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("pcmMulaw").ToLocalChecked());
      break;
    case frameType_pcmS16:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("pcmS16").ToLocalChecked());
      break;
    case frameType_speex:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("speex").ToLocalChecked());
      break;
    case frameType_vorbis:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("vorbis").ToLocalChecked());
      break;
    default:
      puts("不明なフレームタイプでした。");
      return false;
    }
  }
  else if(ttLibC_Frame_isVideo(ttFrame)) {
    ttLibC_Video *video = (ttLibC_Video *)ttFrame;
    Nan::Set(jsFrame, Nan::New("width").ToLocalChecked(),  Nan::New(video->width));
    Nan::Set(jsFrame, Nan::New("height").ToLocalChecked(), Nan::New(video->height));
    switch(video->type) {
    case videoType_key:
      Nan::Set(jsFrame, Nan::New("videoType").ToLocalChecked(), Nan::New("key").ToLocalChecked());
      break;
    case videoType_inner:
      Nan::Set(jsFrame, Nan::New("videoType").ToLocalChecked(), Nan::New("inner").ToLocalChecked());
      break;
    case videoType_info:
      Nan::Set(jsFrame, Nan::New("videoType").ToLocalChecked(), Nan::New("info").ToLocalChecked());
      break;
    default:
      break;
    }
    switch(ttFrame->type) {
    case frameType_bgr:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("bgr").ToLocalChecked());
      break;
    case frameType_flv1:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("flv1").ToLocalChecked());
      break;
    case frameType_h264:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("h264").ToLocalChecked());
      break;
    case frameType_h265:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("h265").ToLocalChecked());
      break;
    case frameType_jpeg:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("jpeg").ToLocalChecked());
      break;
    case frameType_theora:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("theora").ToLocalChecked());
      break;
    case frameType_vp6:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("vp6").ToLocalChecked());
      break;
    case frameType_vp8:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("vp8").ToLocalChecked());
      break;
    case frameType_vp9:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("vp9").ToLocalChecked());
      break;
    case frameType_wmv1:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("wmv1").ToLocalChecked());
      break;
    case frameType_wmv2:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("wmv2").ToLocalChecked());
      break;
    case frameType_yuv420:
      Nan::Set(jsFrame, Nan::New("type").ToLocalChecked(), Nan::New("yuv").ToLocalChecked());
      break;
    default:
      puts("不明なフレームタイプでした。");
      return false;
    }
  }
  else {
    puts("映像でも音声でもないフレームでした。");
    return false;
  }
  Frame *frame = Nan::ObjectWrap::Unwrap<Frame>(jsFrame);
  frame->frame_ = ttFrame;
  frame->isRef_ = true;
  return true;
}

ttLibC_Frame_Type Frame::getFrameType(std::string name) {
  if(name == "aac") {
    return frameType_aac;
  }
  else if(name == "adpcmImaWav") {
    return frameType_adpcm_ima_wav;
  }
  else if(name == "mp3") {
    return frameType_mp3;
  }
  else if(name == "nellymoser") {
    return frameType_nellymoser;
  }
  else if(name == "opus") {
    return frameType_opus;
  }
  else if(name == "pcmAlaw") {
    return frameType_pcm_alaw;
  }
  else if(name == "pcmF32") {
    return frameType_pcmF32;
  }
  else if(name == "pcmMulaw") {
    return frameType_pcm_mulaw;
  }
  else if(name == "pcmS16") {
    return frameType_pcmS16;
  }
  else if(name == "speex") {
    return frameType_speex;
  }
  else if(name == "vorbis") {
    return frameType_vorbis;
  }
  else if(name == "bgr") {
    return frameType_bgr;
  }
  else if(name == "flv1") {
    return frameType_flv1;
  }
  else if(name == "h264") {
    return frameType_h264;
  }
  else if(name == "h265") {
    return frameType_h265;
  }
  else if(name == "jpeg") {
    return frameType_jpeg;
  }
  else if(name == "theora") {
    return frameType_theora;
  }
  else if(name == "vp6") {
    return frameType_vp6;
  }
  else if(name == "vp8") {
    return frameType_vp8;
  }
  else if(name == "vp9") {
    return frameType_vp9;
  }
  else if(name == "wmv1") {
    return frameType_wmv1;
  }
  else if(name == "wmv2") {
    return frameType_wmv2;
  }
  else if(name == "yuv") {
    return frameType_yuv420;
  }
  return frameType_unknown;
}

ttLibC_Frame *Frame::refFrame(Local<Object> jsFrame) {
  Frame *frame = Nan::ObjectWrap::Unwrap<Frame>(jsFrame);
  return frame->frame_;
}

NAN_METHOD(Frame::New) {
  /*
  何もないところからつくって　-> ttLibC_Frameを追加して扱う
  JsのFrameオブジェクトから、内部のttLibC_Frameのcloneをつくって、それを利用するものを作っておいたら幸せになれそうではある。
  コンストラクタではなくて、cloneというメソッドを準備しておく方がいいかな。
  BinaryデータからFrameオブジェクトを復元する動作もほしい。
  これもFrameを空から再生成して・・・とした方がいいかね。
   */
  if(info.IsConstructCall()) {
    Frame *frame = new Frame();
    frame->Wrap(info.This());
    info.GetReturnValue().Set(info.This());
  }
  else {
    info.GetReturnValue().Set(newInstance());
  }
}

NAN_METHOD(Frame::GetBinaryBuffer) {
  Frame *frame = Nan::ObjectWrap::Unwrap<Frame>(info.Holder());
  if(frame->frame_ == NULL) {
    info.GetReturnValue().Set(Nan::Null());
    return;
  }
  info.GetReturnValue().Set(Nan::CopyBuffer((char *)frame->frame_->data, frame->frame_->buffer_size).ToLocalChecked());
}

NAN_METHOD(Frame::Clone) {
  Frame *frame = Nan::ObjectWrap::Unwrap<Frame>(info.Holder());
  Local<Object> jsFrame = Frame::newInstance();
  Frame::setFrame(jsFrame, ttLibC_Frame_clone(NULL, frame->frame_));
  Frame *newFrame = Nan::ObjectWrap::Unwrap<Frame>(jsFrame);
  newFrame->isRef_ = false;
  info.GetReturnValue().Set(jsFrame);
}

Frame::Frame() {
  frame_ = NULL;
  isRef_ = false;
}

Frame::~Frame() {
  if(!isRef_) {
    puts("解放します。");
    ttLibC_Frame_close(&frame_);
  }
}
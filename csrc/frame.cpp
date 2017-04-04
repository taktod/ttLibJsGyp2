#include "frame.h"

#include <stdlib.h>
#include <string.h>

#include <ttLibC/frame/audio/audio.h>
#include <ttLibC/frame/video/video.h>

void Frame::classInit(Local<Object> target) {
  Local<FunctionTemplate> tpl = Nan::New<FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Frame").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  constructor().Reset(Nan::GetFunction(tpl).ToLocalChecked());
  Nan::Set(
    target,
    Nan::New("Frame").ToLocalChecked(),
    Nan::GetFunction(tpl).ToLocalChecked());
}

Local<Object> Frame::newInstance() {
  return Nan::NewInstance(Nan::New(constructor()), 0, NULL).ToLocalChecked();
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
  Nan::Set(jsFrame, Nan::New("id").ToLocalChecked(), Nan::New(ttFrame->id));
  if(ttLibC_Frame_isAudio(ttFrame)) {
    ttLibC_Audio *audio = (ttLibC_Audio *)ttFrame;
    Nan::Set(jsFrame, Nan::New("sampleRate").ToLocalChecked(), Nan::New(audio->sample_rate));
    Nan::Set(jsFrame, Nan::New("sampleNum").ToLocalChecked(), Nan::New(audio->sample_num));
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
    Nan::Set(jsFrame, Nan::New("width").ToLocalChecked(), Nan::New(video->width));
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

ttLibC_Frame_Type Frame::getFrameType(const char *name) {
  int nameLength = strlen(name);
  switch(nameLength) {
  case 3:
    if(strcmp(name, "aac") == 0) {
      return frameType_aac;
    }
    else if(strcmp(name, "mp3") == 0) {
      return frameType_mp3;
    }
    else if(strcmp(name, "bgr") == 0) {
      return frameType_bgr;
    }
    else if(strcmp(name, "vp6") == 0) {
      return frameType_vp6;
    }
    else if(strcmp(name, "vp8") == 0) {
      return frameType_vp8;
    }
    else if(strcmp(name, "vp9") == 0) {
      return frameType_vp9;
    }
    else if(strcmp(name, "yuv") == 0) {
      return frameType_yuv420;
    }
    break;
  case 4:
    if(strcmp(name, "opus") == 0) {
      return frameType_opus;
    }
    else if(strcmp(name, "flv1") == 0) {
      return frameType_flv1;
    }
    else if(strcmp(name, "h264") == 0) {
      return frameType_h264;
    }
    else if(strcmp(name, "h265") == 0) {
      return frameType_h265;
    }
    else if(strcmp(name, "jpeg") == 0) {
      return frameType_jpeg;
    }
    else if(strcmp(name, "wmv1") == 0) {
      return frameType_wmv1;
    }
    else if(strcmp(name, "wmv2") == 0) {
      return frameType_wmv2;
    }
    break;
  case 5:
    if(strcmp(name, "speex") == 0) {
      return frameType_speex;
    }
    break;
  case 6:
    if(strcmp(name, "pcmF32") == 0) {
      return frameType_pcmF32;
    }
    else if(strcmp(name, "pcmS16") == 0) {
      return frameType_pcmS16;
    }
    else if(strcmp(name, "vorbis") == 0) {
      return frameType_vorbis;
    }
    else if(strcmp(name, "theora") == 0) {
      return frameType_theora;
    }
    break;
  case 7:
    if(strcmp(name, "pcmAlaw") == 0) {
      return frameType_pcm_alaw;
    }
    break;
  case 8:
    if(strcmp(name, "pcmMulaw") == 0) {
      return frameType_pcm_mulaw;
    }
    break;
  case 10:
    if(strcmp(name, "nellymoser") == 0) {
      return frameType_nellymoser;
    }
    break;
  case 11:
    if(strcmp(name, "adpcmImaWav") == 0) {
      return frameType_adpcm_ima_wav;
    }
    break;
  default:
    break;
  }
  return frameType_unknown;
}

ttLibC_Frame *Frame::refFrame(Local<Object> jsFrame) {
  Frame *frame = Nan::ObjectWrap::Unwrap<Frame>(jsFrame);
  return frame->frame_;
}

NAN_METHOD(Frame::New) {
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

}

Frame::Frame() {
  frame_ = NULL;
  isRef_ = false;
}

Frame::~Frame() {
  if(!isRef_) {
    ttLibC_Frame_close(&frame_);
  }
}
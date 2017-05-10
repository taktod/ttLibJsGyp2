/*
 * mmAudioLoopbackUtil.h
 *
 * Created on: 2017/05/03
 *     Author: taktod
 */

#ifndef TTLIBC_UTIL_MMAUDIOLOOPBACKUTIL_H_
#define TTLIBC_UTIL_MMAUDIOLOOPBACKUTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <ttLibC/frame/frame.h>
typedef struct ttLibC_Util_MmAudioLoopback {
  int hogehoge;
} ttLibC_Util_MmAudioLoopback;

typedef ttLibC_Util_MmAudioLoopback ttLibC_MmAudioLoopback;

typedef bool (*ttLibC_MmAudioLoopbackDeviceNameFunc)(void *ptr, const char *name);
typedef bool (*ttLibC_MmAudioLoopbackFrameFunc)(void *ptr, ttLibC_Frame *frame);

// これ・・・const charじゃない方がいいのかな。
bool ttLibC_MmAudioLoopback_getDeviceNames(
  const char *locale,
  ttLibC_MmAudioLoopbackDeviceNameFunc callback,
  void *ptr);

/**
 * deviceNameがNULLの場合はデフォルト動作とする。　
 */
ttLibC_MmAudioLoopback *ttLibC_MmAudioLoopback_make(
  const char *locale,
  const char *deviceName);

bool ttLibC_MmAudioLoopback_queryFrame(
  ttLibC_MmAudioLoopback *device,
  ttLibC_MmAudioLoopbackFrameFunc callback,
  void *ptr);

void ttLibC_MmAudioLoopback_close(ttLibC_MmAudioLoopback **device);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* TTLIBC_UTIL_MMAUDIOLOOPBACKUTIL_H_ */


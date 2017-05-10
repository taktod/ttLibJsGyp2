/*
 * clean.h
 * 
 * Created on: 2017/05/03
 *     Author: taktod
 */

#ifndef TTLIBC_UTIL_CLEAN_H_
#define TTLIBC_UTIL_CLEAN_H_

#ifdef __cplusplus
class CoUninitializeOnExit {
public:
  ~CoUninitializeOnExit() {
    CoUninitialize();
  }
};

class CloseHandleOnExit {
public:
  CloseHandleOnExit(HANDLE h) : m_h(h) {}
  ~CloseHandleOnExit() {
    if(!CloseHandle(m_h)) {
      puts("close handle failed.");
    }
  }
private:
  HANDLE m_h;
};

class ReleaseOnExit {
public:
  ReleaseOnExit(IUnknown *p) : m_p(p) {}
  ~ReleaseOnExit() {
    if(m_p != NULL) {
      m_p->Release();
      m_p = NULL;
    }
  }
private:
  IUnknown *m_p;
};

#endif

#endif /* TTLIBC_UTIL_CLEAN_H_ */
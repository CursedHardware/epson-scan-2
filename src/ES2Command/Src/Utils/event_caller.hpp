#pragma once

#ifdef WIN32
#include <Windows.h>
#include <process.h>
#else
#include <pthread.h>
#endif
#include <mutex>
#include <chrono>
#include <functional>
#include <atomic>
#include <condition_variable>

class event_caller
{
public:
  event_caller(std::function<void()> call, int interval_msec = 1000)
  : call_(call)
  , interval_msec_(interval_msec)
  , task_(0)
  , is_exit_(false)
  , is_stop_(false)
  {
  }
  virtual ~event_caller()
  {
    try{
      if(task_ != 0){
        while(is_exit_.exchange(true)){}
        cond_.notify_one();
#ifdef WIN32
        ES2WaitForSingleObject(task_, INFINITY);
        CloseHandle(task_);
#else
        pthread_join(task_, NULL);
        task_ = (pthread_t)NULL;
#endif
      }
    }catch(...){
    }
  }

  void finalize()
  {
      try{
          if(task_ != 0){
              while(is_exit_.exchange(true)){}
              cond_.notify_one();
#ifdef WIN32
              ES2WaitForSingleObject(task_, INFINITY);
        CloseHandle(task_);
#else
              pthread_join(task_, NULL);
              task_ = (pthread_t)NULL;
#endif
          }
      }catch(...){
      }
  }

  void stop(){
    while(is_stop_.exchange(true)){}
  }

  void start()
  {
    while(is_stop_.exchange(false)){}
    if (task_ == 0){
#ifdef WIN32
    unsigned threadID;
    task_ = (HANDLE)_beginthreadex(NULL, 0, &event_caller::event_loop_, (void*)this, 0, &threadID);
#else
      pthread_create(&task_, NULL, &event_caller::event_loop_, (void*)this);
#endif
    }
  }

  bool isValidTask()
  {
    return  task_ != 0;
  }


protected:
#ifdef WIN32
  static unsigned __stdcall event_loop_(void* param){
#else
  static void* event_loop_(void* param){
#endif
    event_caller* pThis = reinterpret_cast<event_caller*>(param);

    bool is_exit = pThis->is_exit_.load();
    bool is_stop = pThis->is_stop_.load();

    while (pThis->call_ && !is_exit) {
      try{

        using namespace std::chrono;
        steady_clock::time_point tp = steady_clock::now() + milliseconds(pThis->interval_msec_);
        {
          std::unique_lock<std::mutex> lk(pThis->mtx_);
          pThis->cond_.wait_until(lk, tp);
        }

        is_stop = pThis->is_stop_.load();
        is_exit = pThis->is_exit_.load();

        if (!is_stop && !is_exit){
          pThis->call_();
        }
      } catch(...){
        break;
      }
    }
#ifdef _WIN32
  _endthreadex(0);
  return 0;
#else
    return (void*)0;
#endif
  }


private:
  std::function<void()> call_;
  int interval_msec_;

#ifdef WIN32
  HANDLE task_;
#else
  pthread_t task_;
#endif
  std::atomic_bool is_exit_;
  std::atomic_bool is_stop_;

  std::mutex mtx_;

  std::condition_variable cond_;
};

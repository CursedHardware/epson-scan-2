#pragma once

#include <deque>
#include <future>
#include <thread>
#include <chrono>
#include <functional>
#include <stdexcept>
#include <sys/ipc.h>

#include "../IInterface.h"
#include "ipc_header.hpp"
#include "Utils/shared_memory.hpp"
#include "Utils/semaphore.hpp"
#include "Utils/ESDebugLog.h"
namespace ipc {

class ipc_interrupt
{
public:
    typedef shared_memory<struct ipc_interrupt_event_data> ipc_shared_memory;
public:
  ipc_interrupt(IInterfaceDelegate *delegate, 
                const std::string shared_memory_file, int shared_memory_id,
                int semaphore_key,
                int interval_msec = 1)
  : delegate_(delegate)
  , interval_msec_(interval_msec)
  , is_exit_(false)
  , shared_memory_(new ipc_shared_memory(shared_memory_file.c_str(), shared_memory_id, true))
  , semaphore_(new semaphore(semaphore_key, true, true))
  {
    if (!shared_memory_ || !semaphore_){
      shared_memory_ = nullptr;
      semaphore_ = nullptr;
      
      throw std::runtime_error("ipc_interrupt initialize failed");
    }
  }
  virtual ~ipc_interrupt()
  {
    stop();
  }

  void start()
  {
    ES_LOG_TRACE_FUNC();
    task_ = std::thread([this] { event_loop_(); });
  }

  void stop()
  {
    ES_LOG_TRACE_FUNC();
    try{
      if (task_.joinable()){
        is_exit_ = true;
        task_.join();
        shared_memory_ = nullptr;
        semaphore_ = nullptr;

        queue_.clear();
      }
    }catch(...){
    }
  }

  key_t sem_key() const { return (semaphore_ ? semaphore_->key() : -1);}

protected:
  void event_loop_()
  {
    while(!is_exit_ && semaphore_ && shared_memory_)
    {
      int ret = semaphore_->wait_and_lock(interval_msec_);
      if (ret == EAGAIN){
         continue;
       } else if (ret == EIDRM){  
         break;
       } else if (ret != 0){
         ES_ERROR_LOG(ES_STRING("interrupt semaphore err (%d)\n"), ret);
         break;
      } else {
       if (!is_exit_ && shared_memory_)
        {
          DealInterruptEvent(shared_memory_->data());
        }
        semaphore_->unlock();
      }
    }
  }

  bool is_button_event(struct ipc_interrupt_event_data &event_data)
  {
    switch(event_data._type)
    {
    case event_did_press_button:
    case event_request_start_scanning:
    case event_request_stop_scanning:
    case event_request_start_or_stop:
    case event_request_stop:
      return true;
    default:
      return false;
    }
  }

  void DealInterruptEvent(struct ipc_interrupt_event_data &event_data)
  {
    ES_LOG_TRACE_FUNC();
    if (is_button_event(event_data)){
      std::unique_lock<std::mutex> lk(mtx_);
      queue_.push_back(event_data);
    } else {
      NotifyInterruptEvent(event_data);
    }
  }

  struct ipc_interrupt_event_data PopEvent()
  {
    struct ipc_interrupt_event_data event_data{};
    {
      std::unique_lock<std::mutex> lk(mtx_);
      if (!queue_.empty()){
        event_data = queue_.front();
        queue_.pop_front();
      }
    }
    return event_data;
  }

  void NotifyInterruptEvent(struct ipc_interrupt_event_data &event_data)
  {
    if (!delegate_){
      return;
    }

    switch(event_data._type)
    {
    case event_reserved_by_host:
      {
        ES_INFO_LOG(ES_STRING("event_reserved_by_host"));
        std::string address(&event_data._data[0], &event_data._data[IPC_INTERRUPT_DATA_SIZE-1]);
        delegate_->IsReservedByHost(nullptr, address.c_str());
      }
      break;
	case event_did_press_button:
      {
        ES_INFO_LOG(ES_STRING("event_did_press_button"));
        UInt8 buttonNumber = static_cast<UInt8>(event_data._data[0]);
        delegate_->DidPressButton(buttonNumber);
      }
      break;
    case event_request_start_scanning:
      {
        ES_INFO_LOG(ES_STRING("event_request_start_scanning"));
        delegate_->DidRequestStartScanning();
      }
      break;
    case event_request_stop_scanning:
      {
        ES_INFO_LOG(ES_STRING("event_request_stop_scanning"));
        delegate_->DidRequestStopScanning();
      }
      break;
    case event_request_start_or_stop:
      {
        ES_INFO_LOG(ES_STRING("event_request_start_or_stop"));
        delegate_->DidRequestStartOrStop();
      }
      break;
    case event_request_stop:
      {
        ES_INFO_LOG(ES_STRING("event_request_stop"));
        delegate_->DidRequestStop();
      }
      break;
    case event_did_timeout:
      {
        ES_INFO_LOG(ES_STRING("event_did_timeout"));
        std::thread([&]{delegate_->DidTimeout();}).detach();
      }
      break;
    case event_did_disconnect:
      {
        ES_INFO_LOG(ES_STRING("event_did_disconnect"));
        std::thread([&]{delegate_->DidDisconnect();}).detach();
      }
      break;
    case event_receive_server_err:
      {
        ES_INFO_LOG(ES_STRING("event_receive_server_err"));
        std::thread([&]{delegate_->DidReceiveServerError();}).detach();
      }
      break;
    case event_device_comunication_err:
      {
        ES_INFO_LOG(ES_STRING("event_device_comunication_err"));
        uint32_t err;
        memcpy(&err, &event_data._data[0], sizeof(err));
        err = ntohl(err);
        std::thread([&]{delegate_->DeviceCommunicationError(static_cast<ESErrorCode>(err));}).detach();
      }
      break;
    case ask_is_should_prevent_timeout:
      {
        ES_INFO_LOG(ES_STRING("ask_is_should_prevent_timeout"));
        bool ret = delegate_->ShouldPreventTimeout();
        event_data._recv_result = static_cast<uint32_t>(ret);
      }
      break;
    default:
      break;
    }
  }

public:
  void NotifyInterruptEvent()
  {
    ipc_interrupt_event_data event_data = PopEvent();
    NotifyInterruptEvent(event_data);
  }

private:
  IInterfaceDelegate  *delegate_;
  int interval_msec_;

  std::shared_ptr<ipc_shared_memory> shared_memory_;
  std::shared_ptr<semaphore> semaphore_;

  std::mutex mtx_;
  std::deque<struct ipc_interrupt_event_data> queue_;

  std::thread task_;
  std::atomic_bool is_exit_;
};

}; // namespace ipc

#pragma once

#include <string>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <string.h>
#include <cassert>
#include <stdexcept>

template<class T, size_t SIZE = sizeof(T) >
class shared_memory
{
public:
    shared_memory(const std::string file, int id, bool create)
    : file_(file), id_(id), key_(-1), segment_id_(-1), shared_memory_(nullptr), is_create_(false)
    {
        key_ = ftok(file_.c_str(), id_);
        if(key_ == -1) {
            throw std::runtime_error("Failed to acquire key");
        }

        if (create){
            segment_id_ = shmget(key_,
                                SIZE,
                                IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR);
            is_create_ = true;
        } else {
            segment_id_ = shmget(key_, 0, 0);
        }
        if (segment_id_ == - 1){
            throw std::runtime_error("Failed to acquire segment");
        }

        shared_memory_ = reinterpret_cast<char*>(shmat(segment_id_, 0, 0));
        
        if(is_create_){
            memset(shared_memory_, 0, SIZE);
        }
    }

    ~shared_memory()
    {
        if (shared_memory_){
            shmdt(shared_memory_);
        }   
        if (is_create_ && segment_id_ != -1){
            shmctl (segment_id_, IPC_RMID, 0);
        }
    }

    key_t key() const {return key_;}
    int segment_id() const {return segment_id_;}
    void* memory_ptr() const {return shared_memory_;}
    T& data() { return *reinterpret_cast<T*>(shared_memory_); }

private:
    std::string file_;
    int         id_;
    key_t       key_;

    bool        is_create_;
    int         segment_id_;
    void*       shared_memory_;
};


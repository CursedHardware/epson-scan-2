#pragma once

#include<sys/sem.h>
#include <stdexcept>

#if defined(__GNU_LIBRARY__) && !defined(_SEM_SEMUN_UNDEFINED)
    /* union semun is defined by including <sys/sem.h> */
#else
    /* according to X/OPEN we have to define it ourselves */
    union semun {
        int val;                    /* value for SETVAL */
        struct semid_ds *buf;       /* buffer for IPC_STAT, IPC_SET */
        unsigned short int *array;  /* array for GETALL, SETALL */
        struct seminfo *__buf;      /* buffer for IPC_INFO */
    };
#endif

enum SEMAPHORE_OPERATION
{
    UNLOCK = -1,
    WAIT = 0,
    LOCK = 1,
};

class semaphore 
{
public:
    semaphore(key_t key, bool create, bool lock)
    : key_(key), sem_id_(-1), create_(create)
    {
        int sem_flags = 0666;
        if(create_){
            sem_flags |= (IPC_CREAT | IPC_EXCL);
        }
        sem_id_ = semget(key_, 1, sem_flags);
        if (sem_id_ == -1) {
            if (errno == EEXIST) {
                do {
                    ++key_;
                    sem_id_ = semget(key_, 1, sem_flags);
                    if (sem_id_ == -1 && errno != EEXIST){
                        throw std::runtime_error("Failed to acquire semapore");
                    }
                } while(sem_id_ < 0);
            } else {
                throw std::runtime_error("Failed to acquire semapore");
            }
        }
        if(create_){
            union semun argument;
            unsigned short values[1];
            values[0] = lock ? 1 : 0;
            argument.array = values;
            semctl(sem_id_, 0, SETALL, argument);
        }
    }
    ~semaphore()
    {
        if(create_){
           semctl(sem_id_, 1, IPC_RMID, NULL);
        }
    }
    
    key_t key() const {return key_;}
    int sem_id() const {return sem_id_;}

    void wait()
    {
        sembuf operations[1];
        operations[0].sem_num = 0;
        operations[0].sem_op = WAIT;
        operations[0].sem_flg = SEM_UNDO;
        semop(sem_id_, operations, 1);
    }
    int wait(int timeout_s)
    {
        sembuf operations[1];
        operations[0].sem_num = 0;
        operations[0].sem_op = WAIT;
        operations[0].sem_flg = SEM_UNDO;

        struct timespec time{};
        time.tv_sec = timeout_s;

        if (semtimedop(sem_id_, operations, 1, &time) == -1){
            return errno;
        }
        return 0;
    }
    void lock()
    {
        sembuf operations[1];
        operations[0].sem_num = 0;
        operations[0].sem_op = LOCK;
        operations[0].sem_flg = SEM_UNDO;
        semop(sem_id_, operations, 1);
    }
    void unlock()
    {
        sembuf operations[1];
        operations[0].sem_num = 0;
        operations[0].sem_op = UNLOCK;
        operations[0].sem_flg = SEM_UNDO;
        semop(sem_id_, operations, 1);
    }
    void wait_and_lock()
    {
        sembuf operations[2];
        operations[0].sem_num = 0;
        operations[0].sem_op = WAIT;
        operations[0].sem_flg = SEM_UNDO;
        operations[1].sem_num = 0;
        operations[1].sem_op = LOCK;
        operations[1].sem_flg = SEM_UNDO;
        semop(sem_id_, operations, 2);
    }
    int wait_and_lock(int timeout_s)
    {
        sembuf operations[2];
        operations[0].sem_num = 0;
        operations[0].sem_op = WAIT;
        operations[0].sem_flg = SEM_UNDO;
        operations[1].sem_num = 0;
        operations[1].sem_op = LOCK;
        operations[1].sem_flg = SEM_UNDO;

        struct timespec time{};
        time.tv_sec = timeout_s;

        if (semtimedop(sem_id_, operations, 2, &time) == -1){
            return errno;
        }
        return 0;
    }
private:
    key_t key_;
    int   sem_id_;
    bool  create_;
};
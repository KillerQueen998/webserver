#ifndef LOCKER_H
#define LOCKER_H
#include "pthread.h"
#include "exception"
#include "semaphore.h"
//线程同步机制封装类
class locker{
    public:
        locker(){
            if(pthread_mutex_init(&mutex,NULL) != 0){
                throw std::exception();
            }
        }
        ~locker(){
            pthread_mutex_destroy(&mutex);
        }
        bool lock(){
            return pthread_mutex_lock(&mutex) == 0;
        }
        bool unlock(){
            return pthread_mutex_unlock(&mutex) == 0;
        }
        pthread_mutex_t * get(){
            
           
            return & mutex;
        }
    private:
        pthread_mutex_t mutex;
};

class condition{
    public:
        condition(){
            if(pthread_cond_init(&cond,NULL) != 0){
                throw std::exception();
            }
        }
        bool wait(pthread_mutex_t * mutex){
            return pthread_cond_wait(&cond,mutex) == 0;
        }
        bool signal(){
            return pthread_cond_signal(&cond) == 0;
        }
        pthread_cond_t * get(){
            return &cond;
        }
        ~condition(){
            pthread_cond_destroy(&cond);
        }
    private:
    pthread_cond_t cond;
};

class semi{
    public:
        semi(){
            if(sem_init(&m_sem,0,0) != 0){
                throw std::exception();
            }
        }
        semi(int num){
            if(sem_init(&m_sem,num,0) != 0){
                throw std::exception();
            }
        }
        bool semwait(){
            return sem_wait(&m_sem) == 0;
        }
        bool sempost(){
            return sem_post(&m_sem) == 0;
        }
        sem_t * get(){
            return &m_sem;
        }
        ~semi(){
            sem_destroy(&m_sem);
        }
    private:
        sem_t m_sem;
};
#endif
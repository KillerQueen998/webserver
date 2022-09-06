#ifndef TOOLS_H
#define TOOLS_H

#include "pthread.h"
#include "iostream"
#include "string"
#include "vector"
#include "arpa/inet.h"
#include "fcntl.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "sys/time.h"
#include "pthread.h"
#include "semaphore.h"
#include "exception"
class locker{
    public:
        locker();
        ~locker();
        void lock();
        void unlock();
    private:
        pthread_mutex_t l_mutex;
};


class sem{
    private:
        sem_t s_sem;
    public:
        void sempost();
        void semwait();  
        sem();
        ~sem();

};

class message{
    private:
    public:
};

#endif
#include "tools.h"
locker::locker(){
    if(pthread_mutex_init(&l_mutex,NULL) != 0 ){
        throw std::exception();
    }
}

locker::~locker(){
    pthread_mutex_destroy(&l_mutex);
}

void locker::lock(){
    pthread_mutex_lock(&l_mutex);
}
void locker::unlock(){
    pthread_mutex_unlock(&l_mutex);
}


sem::sem(){
    if(sem_init(&s_sem,0,0) != 0){
        throw std::exception();
    }
}
sem::~sem(){
    sem_destroy(&s_sem);
}
void sem::sempost(){
    sem_wait(&s_sem);
}
void sem::semwait(){
    sem_post(&s_sem);
}
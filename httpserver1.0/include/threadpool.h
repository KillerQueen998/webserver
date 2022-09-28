#ifndef THREADPOOL_H
#define THREADPOOL_H
#include <pthread.h>
#include "vector"
#include "list"
#include "locker.h"

template <typename T>
class threadpool{
    public:
        threadpool(int num = 4,int max = 10000);
        ~threadpool();
        bool addRquirs(T * rquirs);
        void run();
        static void * func(void * args);
    private:
        int threadNum;
        int max_requir_num;
        bool m_stop;
        std::vector<pthread_t> thread_arrry;
        std::list<T *> requireslist;
        locker mutex;
        condition cond;
        semi m_sem; 
};



template <typename T>
threadpool<T>::threadpool(int num ,int max):threadNum(num),max_requir_num(max),m_stop(false){
    
    if(num <= 0 || max <= 0){
        throw std::exception();
    }
    pthread_t thid;
    for (size_t i = 0; i < threadNum; i++)
    {   
        std::cout << "正在创建第" << i << "个线程......." << std::endl;
        
        //func必须是静态的
        if(pthread_create(&thid,NULL,func,this) != 0){
            throw std::exception();
        }
        thread_arrry.push_back(thid);
        if(pthread_detach(thid)){
            throw std::exception();
        }
    }
    
}

template <typename T>
threadpool<T>::~threadpool(){
    m_stop = true; 
}

template <typename T>
bool threadpool<T>::addRquirs(T * require){
    mutex.lock();
    if(requireslist.size() > thread_arrry.size()){
        mutex.unlock();
        return false;
    }
    requireslist.push_back(require);
    mutex.unlock();
    m_sem.sempost();
    return true;
}
template <typename T>
void * threadpool<T>::func(void * args){
    threadpool * pool = (threadpool *)args;
    pool->run();
    return pool;
}
template <typename T>
void threadpool<T>::run(){
    while(!m_stop){
        m_sem.semwait();
        mutex.lock();
        if(requireslist.empty()){
            mutex.unlock();
            continue;
        }
        T * requir = requireslist.back();
        requireslist.pop_back();
        mutex.unlock();

        if (!requir)
        {
            continue;
        }
        requir->process();
        
    }
}
#endif
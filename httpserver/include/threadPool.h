
#ifndef THREADPOOL_H
#define THREADPOOL_H
#include "pthread.h"
#include "vector"
#include "list"
#include "locker.h" 
#include "memory"
#include "Work.h"
template <typename T>
class threadPool{
    public:
        threadPool(int num = 4,int max = 10000);
        ~threadPool();
        bool work_add(std::shared_ptr<T> work);
        
        static void * work(void * args);
        void run();
    private:
        int threadNum;
        int max_work_num;
        
        bool m_stop;

        std::vector<pthread_t> thread_arrry;
        std::list<std::shared_ptr<T>> work_list;
        
        locker mutex;
        semi m_sem; 
};
template <typename T>
threadPool<T>::threadPool(int num ,int max):threadNum(num),max_work_num(max),m_stop(false){
    
    if(num <= 0 || max <= 0){
        throw std::exception();
    }
    pthread_t thid;
    for (size_t i = 0; i < threadNum; i++)
    {   
        std::cout << "正在创建第" << i << "个线程......." << std::endl;

        //func必须是静态的
        if(pthread_create(&thid,NULL,work,this) != 0){
            throw std::exception();
        }
        thread_arrry.push_back(thid);
        if(pthread_detach(thid)){
            throw std::exception();
        }
    }
    
}

template <typename T>
threadPool<T>::~threadPool(){
    m_stop = true; 
}

template <typename T>
void threadPool<T>::run(){
    while (!m_stop)
    {
        
        m_sem.semwait();
        mutex.lock();
        if(work_list.empty()){
            mutex.unlock();
            continue;
        }
        auto requir = work_list.back();
        
        work_list.pop_back();
        mutex.unlock();
        // std::cout << "到这了4" << std::endl;
        if (!requir)
        {
            continue;
        }
        
        auto flag = requir->do_work();
        if(flag){
            // std::cout << "到这了4" << std::endl;
            if(requir->work_type == "work"){
                work_add(requir);
            }
            
        }

        // std::cout << "到这了7" << std::endl;
    }
}

template <typename T>
void * threadPool<T>::work(void * args){
    threadPool * pool = (threadPool *) args;
    pool->run();
    return pool;
    
    
}

template <typename T>
bool threadPool<T>::work_add(std::shared_ptr<T> work){
    
    mutex.lock();
    if(work_list.size() >= max_work_num){
        mutex.unlock();
        return false;
    }
    if(work == NULL){
        mutex.unlock();
        return false;
    }
    work_list.push_back(work);
    // std::cout << "到这了3" << std::endl;
    m_sem.sempost();
    mutex.unlock();
    return true;

}

#endif
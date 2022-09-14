#ifndef THREADPOOL_H
#define THREADPOOL_H
#include "pthread.h"
#include "semaphore.h"
#include "tools.h"
#include "memory"

template <typename T>
class threadpool
{
    private:
        int m_thread_num;
        int m_chet_num;
        static void * m_callback(void * argv);
        std::vector<std::shared_ptr<T>> msg_list;
        void func();
        locker m_lock;
        sem m_sem;
        int msg_size;
    public:
        threadpool(int thread_num,int chet_num);
        bool add_msg(std::shared_ptr<T> msg);
        ~threadpool();
};

template <typename T>
threadpool<T>::threadpool(int thread_num,int chet_num):m_thread_num(thread_num),m_chet_num(chet_num)
{
    msg_size = 0;
    pthread_t pthid;
    for(int i = 0;i < m_thread_num;i++){
        pthread_create(&pthid,NULL,m_callback,this);
        std::cout << "已创建线程:" << i << "....." << std::endl;
        pthread_detach(pthid);
    }
    

}

template <typename T>
threadpool<T>::~threadpool()
{
}

template <typename T>
void * threadpool<T>::m_callback(void * argv){
    ((threadpool *) argv)->func();
    return argv;
}

template <typename T>
void threadpool<T>::func(){
    try
    {
        while(1)
        {
            m_sem.semwait();
            m_lock.lock();
            if(msg_list.empty()){
                m_lock.unlock();
                continue;
            }
            auto msg = msg_list.back(); 
            msg_size -= 1;
            msg_list.pop_back();
            m_lock.unlock();
            bool recved;
            switch (msg->flag)
            {   
                case 1:
                    recved = msg->get();
                    if(recved){
                        auto reac = T::message_queue.front();
                        T::message_queue.pop();
                        auto now_time_list = msg->c_chet_list; 
                        for(auto i : now_time_list){
                            if(i == msg->get_chet_fd()){
                                continue;
                            }else{
                                auto msg_to = msg->copy(reac,i);
                                msg_to->flag = 2;
                                add_msg(msg_to);
                            }
                        }

                        
                    }
                    break;
                case 2:
                    
                    msg->give();
                    break;
                default:
                    break;
            }
            

        }
        
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    
}

template <typename T>
bool threadpool<T>::add_msg(std::shared_ptr<T> msg){
    m_lock.lock();
    if(msg_size + 1 > m_chet_num){
        m_lock.unlock();
        std::cerr << "事件太多了" << std::endl;
        return false;
    }
    if(msg == NULL){
        m_lock.unlock();
        return false;
    }
    msg_list.push_back(msg);
    msg_size += 1;
    m_sem.sempost();
    m_lock.unlock();
    return true;
}

#endif
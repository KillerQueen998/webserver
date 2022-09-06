#ifndef CHET_H
#define CHET_H
#include "arpa/inet.h"
#include "sys/epoll.h"
#include "unistd.h"
#include "sys/stat.h"
#include "string"
#include "fcntl.h"
#include "iostream"
#include "string.h"
#include "regex"
#include "sys/mman.h"
#include "sys/uio.h"
#include "memory"
#include "utility"
#include "queue"

class chet{
    public:
        chet(std::string ip,int port,std::string time,std::string chet_data,int chet_fd,int epoll_fd);
        void give();
        void giveall();
        bool get();
        std::shared_ptr<chet> copy(std::string data,int chet_fd); 
        int flag;
        static std::vector<int> c_chet_list;
        static std::queue<std::string> message_queue;
        int get_chet_fd();
    private:
        //flag 1 接受，2 发送
        int c_chet_fd;
        
        std::string c_ip;
        int c_port;
        
        std::string c_time;
        std::string c_chet_data;
        char c_recv_data[4096];
        std::string c_send_data;
        int c_epoll_fd;

};




#endif
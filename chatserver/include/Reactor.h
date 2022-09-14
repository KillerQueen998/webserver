#ifndef REACTOR_H
#define REACTOR_H
#include "string"
#include "vector"
#include "pthread.h"
#include "sys/socket.h"
#include "threadpool.h"
#include "chet.h"
#include "map"
#include "memory"
extern void epoll_change(int epoll,int fd,int event);
extern void epoll_add(int epoll,int fd,bool one_shot);
extern void epoll_del(int epoll,int fd);

class reactor{
    private:
        threadpool<chet> r_threadpool;
        // std::string r_ip;
        std::map<int,std::shared_ptr<chet>> msg_map;
        int r_port;
        std::vector<int> conn_list;
        int listen_fd;
        static const int con_size = 10000; 
        struct sockaddr_in server_addr;
        int epoll_fd;
        void eraze_in_vector(std::vector<int> & vector,int eraze_fd);
        struct epoll_event ep_event;
        struct sockaddr_in connect_addr;
        void insert_if_no_same(std::vector<int> & vector,int find_fd);
    public:
        bool start();
        reactor(int port,int thread_size);
        ~reactor();
};


#endif
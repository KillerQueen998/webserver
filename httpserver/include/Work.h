#ifndef WORK_H
#define WORK_H
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
#include "../include/locker.h"
#include "sys/uio.h"ta
#include "memory"
#include "utility"

class Work{
    private:
        
        // std::string http_data;
        char http_data[4096];
        
        std::string resourc;
        struct sockaddr_in work_from_addr;
        int work_on_fd;
        int epoll_on_fd;

        std::map<std::string,std::string> http_message;

        enum METHOD{GET=0,POST,HEAD};
        enum CHECK_STATE{ CHECK_STATE_REQUESTLINE = 0,CHECK_STATE_HEADER,CHECH_STATE_CONTENT}; 
        enum CHECK_LINE {LINE_OK = 0,LINE_BAD,LINE_OPEN};
        enum HTTP_CODE {NO_REQUEST = 0,GET_REQUEST,BAD_REQUEST,NO_RESOURCE,FOBIDDEN_REQUEST,FILE_REQUEST,INTERNAL_ERROR,CLOSED_CONNECT};
        HTTP_CODE parse_line(std::string str);
        std::vector<std::string> line;
        bool pares_every_line();
        bool func_get();
        bool func_post();
        void clear_the_data();
        char * share_addr;
        struct iovec iov[2];
        bool is_link;
        int byte_to_send;//将要发送的字节数
        int byte_have_send;//已经发送的字节数
        std::string reac_http_body;
        bool make_reac_http();
        
        
    public:
        int readbuff;
        std::string request_Path;
        Work(int fd,struct sockaddr_in from_addr,int epollnum);
        ~Work();
        // static int work_count;
        bool do_work();
        void set_work_type(std::string w_type);
        std::string work_type;
        bool is_conn;
        locker lock;
        int already_write;
        
        
        
};



#endif
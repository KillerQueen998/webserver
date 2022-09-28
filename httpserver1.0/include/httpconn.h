#ifndef HTTPCONN_H
#define HTTPCONN_H

#include "arpa/inet.h"
#include "sys/epoll.h"
#include "unistd.h"
#include "arpa/inet.h"
#include "string"
#include "fcntl.h"
#include "iostream"
#include "sys/stat.h"
#include "string.h"
#include "regex"
#include "sys/mman.h"
#include "sys/uio.h"
class httpconn{
    public:

        httpconn(){};
        ~httpconn() = default;
        static const int READ_BUF_SIZE = 2048;
        static const int WRITE_BUF_SIZE = 2048;
        void process();
        enum METHOD{GET=0,POST,HEAD};
        enum CHECK_STATE{ CHECK_STATE_REQUESTLINE = 0,CHECK_STATE_HEADER,CHECH_STATE_CONTENT}; 
        enum CHECK_LINE {LINE_OK = 0,LINE_BAD,LINE_OPEN};
        enum HTTP_CODE {NO_REQUEST = 0,GET_REQUEST,BAD_REQUEST,NO_RESOURCE,FOBIDDEN_REQUEST,FILE_REQUEST,INTERNAL_ERROR,CLOSED_CONNECT};
        static int epoll_fd;
        std::string m_real_file;
        CHECK_STATE main_state;
        int m_checked_idx;
        int m_start_line;
        int m_read_line;
        int m_conten_lenth;
        void init();
        void init(int fd,sockaddr_in & addr);
        static int user_count;
        void close_conn();
        bool read();
        bool write();
        METHOD m_method;
        char *m_url;
        char * m_version;
        char * m_host;
        bool m_linker;
        std::string ack_http;
        struct iovec iov[2];
        char * share_addr;
        struct stat m_real_stat;
        bool procss_write(HTTP_CODE react);
        HTTP_CODE procss_read();
        HTTP_CODE parse_request_line(char * text);
        HTTP_CODE parse_headers(char * text);
        HTTP_CODE parse_content(char * text);
        HTTP_CODE do_request();
        CHECK_LINE the_line();
        const char *doc_root = "/home/killerqueen/webserver/resouce";
    private:
        int sock_fd;
        sockaddr_in m_sockaddr;
        char m_read_buf[READ_BUF_SIZE];
        int read_index;
        char * getline(){
            return m_read_buf + m_start_line;
        }
};

#endif
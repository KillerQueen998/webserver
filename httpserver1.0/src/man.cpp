#include "iostream"
#include "unistd.h"
#include "arpa/inet.h"
#include "errno.h"
#include "fcntl.h"
#include "sys/epoll.h"
#include "../include/locker.h"
#include "string"
#include "../include/threadpool.h"
#include "signal.h"
#include "string.h"
#include "exception"
#include "memory"
#include "../include/httpconn.h"
#define MAX_SIZE 65535
#define MAX_LISTEN_SIZE 128
//添加信号捕捉
void addsig(int signum,void handler(int)){
    struct sigaction sa;
    memset(&sa,'\0',sizeof(sa));
    sa.sa_handler = handler;
    sigfillset(&sa.sa_mask);
    sigaction(signum,&sa,NULL);
} 
extern void epoll_change(int epoll,int fd,int event);
extern void epoll_add(int epoll,int fd,bool one_shot);
extern void epoll_del(int epoll,int fd);
int main(int argc,char * argv[]){
    if(argc <= 1){
        std::cout << "请传入端口号" << std::endl;
        exit(0);
    }
    int porti = std::stoi(argv[1]); 
    //对SIGPIPE做处理
    addsig(SIGPIPE,SIG_IGN);
    //创建线程池
    std::unique_ptr<threadpool<httpconn>> pool = NULL; 
    try{
        pool = std::make_unique<threadpool<httpconn>>(4,10000); 
        
    }catch(std::exception ex){
        std::cout << "线程池创建失败" << std::endl;
        exit(-1);
    }catch(...){
        exit(-1);
    }
    
    //创建存储连接的数组
    std::vector<httpconn> conn_list( MAX_SIZE );
    //创建监听数组
    std::vector<struct epoll_event> epoll_list( MAX_LISTEN_SIZE );

    struct sockaddr_in localaddr;
    struct sockaddr_in connectaddr;
    socklen_t sizeofa;
    localaddr.sin_family = AF_INET;
    localaddr.sin_port = htons(porti);
    
    localaddr.sin_addr.s_addr = INADDR_ANY;
    int reusetype = 1;
    int listen_fd = socket(AF_INET,SOCK_STREAM,0);

    setsockopt(listen_fd,SOL_SOCKET,SO_REUSEPORT,&reusetype,4);

    int ref = bind(listen_fd,(struct sockaddr *)&localaddr,sizeof(localaddr));
    if(ref == -1){
        perror("bind");
        close(listen_fd);
        exit(0);
    }

    ref = listen(listen_fd,128);
    if(ref == -1){
        perror("listen");
        close(listen_fd);
        exit(0);
    }
    int epollnum = epoll_create(1998);
    struct epoll_event event;
    event.data.fd = listen_fd;
    event.events = EPOLLIN;
    int connect_fd;
    
    epoll_add(epollnum,listen_fd,false);
    httpconn::epoll_fd = epollnum;
    httpconn::user_count = 0;
    while(1){
        ref = epoll_wait(epollnum,epoll_list.data(),MAX_LISTEN_SIZE,-1);
        if (ref < 0 && (errno != EINTR))
        {
            perror("epoll_wait");
            continue;
        }
        std::cout << httpconn::user_count << std::endl;
        if (ref > 0)
        {
            for(int i = 0;i < ref;i++){
                
                int sockfd = epoll_list[i].data.fd;
                
                if(sockfd == listen_fd){
                    connect_fd = accept(listen_fd,(struct sockaddr *)&connectaddr,&sizeofa);
                    if(connect_fd == -1){
                        perror("ac");
                    }
                    
                    if (httpconn::user_count >= MAX_SIZE)
                    {
                        close(connect_fd);
                        continue;
                    }
                    
                    conn_list[i].init(connect_fd,connectaddr);
                    
                    continue;
                }
                if(epoll_list[i].events & (EPOLLRDHUP |EPOLLHUP | EPOLLERR)){
                    //异常断开
                    conn_list[i].close_conn();
                }else if(epoll_list[i].events & EPOLLIN){
                    //开线程进行读操作
                    //这里用的reactor所以主线程读
                    
                    if(conn_list[i].read()){
                        bool h = pool->addRquirs(conn_list.data()+i);
                        
                       if(!(pool->addRquirs(conn_list.data()+i))){
                            std::cout << "有问题" <<std::endl;
                            continue;
                        }
                    }else{
                        conn_list[i].close_conn();
                    }
                }else if(epoll_list[i].events & EPOLLOUT){
                    //开线程进行写操作
                    if(!conn_list[i].write()){
                       conn_list[i].close_conn();
                    }
                }

            }
        }
        
    }
    close(listen_fd);
    close(epollnum);
    
}
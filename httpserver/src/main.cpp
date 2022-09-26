#include "iostream"
#include "unistd.h"
#include "arpa/inet.h"
#include "errno.h"
#include "fcntl.h"
#include "sys/epoll.h"
#include "../include/locker.h"
#include "../include/threadPool.h"
#include "../include/Work.h"
#include "string"
#include "signal.h"
#include "string.h"
#include "exception"
#include "memory"
#include "vector"
#include "map"

#define MAX_CONN_SIZE 65535
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
    // char ip[16] = "172.17.34.242";
    if(argc <= 1){
        std::cout << "请传入端口号" << std::endl;
        exit(0);
    }
    int porti = std::stoi(argv[1]); 
    //对SIGPIPE做处理
    addsig(SIGPIPE,SIG_IGN);

    //创建线程池
    std::unique_ptr<threadPool<Work>> pool = NULL; 

    try{
        pool = std::make_unique<threadPool<Work>>(4,10000); 
        
    }catch(std::exception ex){
        std::cout << "线程池创建失败" << std::endl;
        exit(-1);
    }catch(...){
        exit(-1);
    }
    
    
    //创建监听数组
    std::vector<struct epoll_event> epoll_list( MAX_LISTEN_SIZE );
    //创建连接Map
    std::map<int,std::shared_ptr<Work>> connect_map;

    struct sockaddr_in localaddr;
    struct sockaddr_in connectaddr;
    socklen_t sizeofa;
    localaddr.sin_family = AF_INET;
    localaddr.sin_port = htons(porti);


    // inet_pton(AF_INET,ip,&localaddr.sin_addr.s_addr);
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

    
    while(1){
        ref = epoll_wait(epollnum,epoll_list.data(),MAX_LISTEN_SIZE,-1);
        if (ref < 0 && (errno != EINTR)){
            perror("epoll_wait");
            continue;
        }
        if (ref > 0){
            //监听epoll做io复用
            for(int i = 0;i < ref;i++){
                int changed_fd = epoll_list[i].data.fd;
                if(changed_fd == listen_fd){
                    connect_fd = accept(listen_fd,(struct sockaddr *)&connectaddr,&sizeofa);
                    epoll_add(epollnum,connect_fd,true);
                    if(connect_fd >= MAX_CONN_SIZE){
                        std::cout << "连接太多，请等待" << std::endl;
                        continue;
                    }
                    std::cout << "到这了0 + fd" << connect_fd << std::endl;
                    auto work = std::make_shared<Work>(connect_fd,connectaddr,epollnum);
                    // if (!(connect_map.find(connect_fd) == connect_map.end()))
                    // {
                    //     if(connect_map[connect_fd]->is_conn){
                    //         (connect_map[connect_fd]->lock).lock();
                    //         connect_map[connect_fd] = work;
                    //         (connect_map[connect_fd]->lock).unlock();
                    //     }
                    // }
                    
                    connect_map[connect_fd] = work;
                    continue;
                }
                if(epoll_list[i].events & (EPOLLRDHUP |EPOLLHUP | EPOLLERR)){
                    //异常断开
                    std::cout << "断开" << std::endl;
                    epoll_del(epollnum,epoll_list[i].data.fd);
                    connect_map[epoll_list[i].data.fd] = NULL;
                    close(epoll_list[i].data.fd);
                }else if(epoll_list[i].events & EPOLLIN){
                    
                    std::cout << "读" << epoll_list[i].data.fd  << std::endl;
                    connect_map[epoll_list[i].data.fd]->set_work_type("read");
                    
                    bool reac = pool->work_add(connect_map[epoll_list[i].data.fd]);
                    if(!reac){
                        std::cout << "事件太多了" << std::endl;
                        epoll_change(epollnum,epoll_list[i].data.fd,EPOLLIN);
                    }
                }else if(epoll_list[i].events & EPOLLOUT){
                    std::cout << "写" << epoll_list[i].data.fd  << std::endl;
                    connect_map[epoll_list[i].data.fd]->set_work_type("write");
                    bool reac = pool->work_add(connect_map[epoll_list[i].data.fd]);
                    if(!reac){
                        std::cout << "事件太多了" << std::endl;
                        epoll_change(epollnum,epoll_list[i].data.fd,EPOLLIN);
                    }
                }
                
            }
        }
    }
    close(epollnum);
    close(listen_fd);
}
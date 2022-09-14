#include "reactor.h"

bool reactor::start(){
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(r_port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    // inet_pton(AF_INET,r_ip.data(),&server_addr.sin_family);
    listen_fd = socket(AF_INET,SOCK_STREAM,0);
    int reusetype = 1;

    setsockopt(listen_fd,SOL_SOCKET,SO_REUSEPORT,&reusetype,4);
    
    socklen_t in;
    
    int ref = bind(listen_fd,(sockaddr *)&server_addr,sizeof(server_addr));
    if(ref == -1){
        perror("bind");
        close(listen_fd);
        exit(0);
    }
    listen(listen_fd,con_size);

    // ep_event.data.fd = listen_fd;
    // ep_event.events = EPOLLIN;
    epoll_fd = epoll_create(1998);
    epoll_add(epoll_fd,listen_fd,false);
    std::vector<struct epoll_event> ep_event_list(con_size);
    char in_ip[16];
    int fdnow;
    while(1){
        ref = epoll_wait(epoll_fd,ep_event_list.data(),con_size,-1);
        if (ref < 0 && (errno != EINTR)){
            perror("epoll_wait");
            std::cout << ep_event_list.size() << std::endl;
            continue;
        }
        if(ref > 0){
            
            for(auto iter : ep_event_list){
                fdnow = iter.data.fd;
                if(fdnow == listen_fd){
                    fdnow = accept(listen_fd,(sockaddr *)&connect_addr,&in);
                    
                    if(fdnow >= con_size){
                        std::cout << "连接太多，请等待" << std::endl;
                        continue;
                    }
                    insert_if_no_same(chet::c_chet_list,fdnow);
                    epoll_add(epoll_fd,fdnow,true);
                    inet_ntop(AF_INET,&connect_addr.sin_addr,in_ip,16);
                    auto the_chet = std::make_shared<chet>(in_ip,ntohs(connect_addr.sin_port),"","",fdnow,epoll_fd);

                    msg_map[fdnow] = the_chet;
                    continue;
                }

                if(iter.events & (EPOLLRDHUP |EPOLLHUP | EPOLLERR)){
                    //异常断开
                    std::cout << "断开" << std::endl;
                    epoll_del(epoll_fd,fdnow);
                    if(chet::c_chet_list.size() != 0){
                        eraze_in_vector(chet::c_chet_list,fdnow);
                    }
                    for(auto iter:chet::c_chet_list){
                        std::cout << iter << std::endl;
                    }
                    msg_map[fdnow] = NULL;
                    close(fdnow);
                }else if(iter.events & EPOLLIN){
                    std::cout << "读" << fdnow  << std::endl;
                    msg_map[fdnow]->flag = 1;
                    bool reac = r_threadpool.add_msg(msg_map[fdnow]);
                    if(!reac){
                        std::cout << "读事件入队失败" << std::endl;
                        epoll_change(epoll_fd,fdnow,EPOLLIN);
                    }
                // }else if(iter.events & EPOLLOUT){
                //     std::cout << "写" << fdnow  << std::endl;
                //     msg_map[fdnow]->flag = 2;
                //     bool reac = r_threadpool.add_msg(msg_map[fdnow]);
                //     if(!reac){
                //         std::cout << "写事件入队列失败" << std::endl;
                //         epoll_change(epoll_fd,fdnow,EPOLLIN);
                //     }
                }


            }
        }
        
    }

    close(listen_fd);
    close(epoll_fd);
    return false;
}

void reactor::eraze_in_vector(std::vector<int> & vector,int eraze_fd){
    auto rm = vector.cbegin();
    for(auto i = vector.cbegin();i < vector.cend();i++){
        if(*i == eraze_fd){
            rm = i;
        }
    }
    if(rm != vector.cbegin()){
        vector.erase(rm);
    }
    
}

reactor::reactor(int port,int thread_size):r_port(port),r_threadpool(thread_size,con_size){

}

void reactor::insert_if_no_same(std::vector<int> & vector,int find_fd){
    auto it = find(vector.begin(),vector.end(),find_fd);
    if(it == vector.end()){
        vector.push_back(find_fd);
    }
}

reactor::~reactor(){

}


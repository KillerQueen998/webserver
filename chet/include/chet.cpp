#include "chet.h"
std::vector<int> chet::c_chet_list;
std::queue<std::string> chet::message_queue;

void setfdNoblocking(int fd){
    int flag = fcntl(fd,F_GETFL,NULL);
    flag |= O_NONBLOCK;
    fcntl(fd,F_SETFL,flag);
}
void epoll_add(int epoll,int fd,bool one_shot){
    struct epoll_event event;
    event.data.fd = fd;
    
    
    event.events = EPOLLIN|EPOLLRDHUP;
    if (one_shot)
    {
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl(epoll,EPOLL_CTL_ADD,fd,&event);
    setfdNoblocking(fd);
}
void epoll_del(int epoll,int fd){
    struct epoll_event event;
    epoll_ctl(epoll,EPOLL_CTL_DEL,fd,&event);
    close(fd);
}

void epoll_change(int epoll,int fd,int evet){
    struct epoll_event event;
    event.data.fd = fd;
    event.events = evet | EPOLLONESHOT |EPOLLRDHUP;
    epoll_ctl(epoll,EPOLL_CTL_MOD,fd,&event);
}

chet::chet(std::string ip,int port,std::string time,std::string chet_data,int chet_fd,int epoll_fd):c_ip(ip),c_port(port),c_time(time),c_chet_data(chet_data),c_chet_fd(chet_fd),c_epoll_fd(epoll_fd){

}

void chet::give(){

    send(c_chet_fd,c_chet_data.data(),c_chet_data.size(),0);
    std::cout << "发送到" << c_chet_fd << std::endl;
    epoll_change(c_epoll_fd,c_chet_fd,EPOLLIN);
}
void chet::giveall(){

}
bool chet::get(){
    while(1){
        int ref = recv(c_chet_fd,c_recv_data,4096,0);
        if(ref == -1){
            if(errno == EAGAIN || errno == EWOULDBLOCK){
                // lock.unlock();
                std::cout << "读到了" << c_recv_data << std::endl;
                chet::message_queue.push(c_recv_data);
                epoll_change(c_epoll_fd,c_chet_fd,EPOLLIN);
                memset(c_recv_data,'\0',sizeof(c_recv_data));
                return true;
                // std::cout << "读到了 接下来做work" << std::endl;
                break;
            }
            std::cout << "没读到 断开连接" << std::endl;
            return false;
        }else if (ref == 0)
        {
            
            std::cout << "没读到 断开连接" << std::endl;
            // lock.unlock();
            return false;
        }
        } 
        
        
        // epoll_change(c_epoll_fd,c_chet_fd,EPOLLOUT);
}
    


std::shared_ptr<chet> chet::copy(std::string data,int chet_fd){
    return std::make_shared<chet>(this->c_ip,this->c_port,this->c_time,data,chet_fd,this->c_epoll_fd);
}

int chet::get_chet_fd(){
    return c_chet_fd;
}
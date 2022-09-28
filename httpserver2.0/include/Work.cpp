#include "Work.h"

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

bool Work::do_work(){
    
    if(work_type == "read"){
        // lock.lock();
        while(true){
            int ref = recv(work_on_fd,http_data + readbuff,4096 - readbuff,0);
            readbuff += ref;
            
            if(ref == -1){
                if(errno == EAGAIN || errno == EWOULDBLOCK){
                    work_type = "work";
                    // lock.unlock();
                    std::cout << "读到了 接下来做work" << std::endl;
                    break;
                }
                std::cout << "没读到 断开连接" << std::endl;
                is_conn = false;
                clear_the_data();
                // lock.unlock();
                return false;
            }else if (ref == 0)
            {
                is_conn = false;
                clear_the_data();
                std::cout << "没读到 断开连接" << std::endl;
                // lock.unlock();
                return false;
            }
        } 
    }else if(work_type == "write"){
        int temp;
        
        
        // lock.lock();
        while (1)
        {
            temp = send(work_on_fd,reac_http_body.data()+already_write,reac_http_body.size()-already_write,0);
            std::cout << "开始写" << std::endl;
            std::cout << reac_http_body << std::endl;
            // std::cout << "写的数据是:\n" << reac_http_body.substr(begin+already,temp)  << std::endl;
            already_write += temp;
            std::cout << temp << "al" << already_write << std::endl;
            // byte_have_send += temp;
            // byte_to_send -= temp;
            if(temp <= -1){
                if(errno == EAGAIN){
                    std::cout << "写完了 再试一次" << std::endl;
                    epoll_change(epoll_on_fd,work_on_fd,EPOLLOUT);
                    // lock.unlock();
                    return true;
                }
                std::cout << "没写完1" << std::endl;
                // munmap(share_addr,iov[1].iov_len);
                // lock.unlock();
                return false;
            }
            if(temp == 0){
                is_conn = false;
                std::cout << "连接关闭" << std::endl;
                clear_the_data();
                // lock.unlock();
                epoll_change(epoll_on_fd,work_on_fd,EPOLLIN);
                return false;
            }
            if(already_write == reac_http_body.size()){
                std::cout << "写完了" << std::endl;
                epoll_change(epoll_on_fd,work_on_fd,EPOLLIN);
            }
            // if(byte_have_send >= iov[0].iov_len){
            //     iov[0].iov_len = 0;
            //     iov[1].iov_base = share_addr + (byte_have_send - reac_http_body.size());
            //     iov[1].iov_len = byte_to_send;
            // }else{
            //     iov[0].iov_base = share_addr + byte_have_send;
            //     iov[0].iov_len = iov[0].iov_len - temp;
            // }
            // if (byte_to_send <= 0)
            // {
                
            //     munmap(share_addr,iov[1].iov_len);
            //     if (!is_link)
            //     {
            //         clear_the_data();
            //     }
                
            //     return true;
            // }
            
        }
        
        
        return true;
        
    }else if(work_type == "work"){

        //将字符串 分行
        // lock.lock();
        std::cout << "开始work" << std::endl;
        std::cout << "读到了\n" << http_data << std::endl;
        auto http_state = parse_line(http_data);
        
        if(http_state == BAD_REQUEST || line.size() <= 1){
            std::cout << "work时 请求出错 重来一次" << std::endl;
            clear_the_data();
            epoll_change(work_on_fd,epoll_on_fd,EPOLLIN);
            // lock.unlock();
            return false;
        }else{
            //解析请求内容
            auto reac = pares_every_line();
            if(!reac){
                std::cout << "work时 请求出错 重来一次" << std::endl;
                clear_the_data();
                epoll_change(work_on_fd,epoll_on_fd,EPOLLIN);
                // lock.unlock();
            }
            //制作响应体
            make_reac_http();
            work_type = "write";
            epoll_change(epoll_on_fd,work_on_fd,EPOLLOUT);
            // lock.unlock();
            
        }
        
        

    }
    return true;
}

bool Work::pares_every_line(){
    //解析行
    std::smatch match;
    std::regex patten("([[:print:]]+) ([[:print:]]+) ([[:print:]]+)");
    regex_search(line[0],match,patten);
    if(match.size() != 4){
        return false;
    }
    if (match.str(1) == "GET")
    {
        http_message["func"] = "GET";
    }else if(match.str(1) == "POST"){
        http_message["func"] = "POST";
    }else if(match.str(1) == "HEAD"){
        http_message["func"] = "HEAD";
    }else{
        return false;
    }

    
    http_message["version"] = match.str(3);
    http_message["url"] = match.str(2);
    patten = "http";
    regex_search(http_message["url"],match,patten);
    if (match.size() > 0)
    {
       patten = "[[:print:]]+://([[:print:]]+\\d)([[:print:]]+)";
       regex_search(http_message["url"],match,patten);
       http_message["url"] = match.str(2).data();
    }
    
    //解析头
    for(int num = 1;num < line.size();num++){
        if(line[num].size() == 0){
            break;
        }
        patten = "([[:print:]]+): ([[:print:]]+)";
        regex_search(line[num],match,patten);
        if(match.size() < 3){
            return false;
        }else{
            http_message[match.str(1)] = match.str(2);
        }
    }
    //解析体
    return true;
}

bool Work::make_reac_http(){
    bool return_value = false;
    std::string url = resourc+http_message["url"];
    std::string code;
    struct stat m_real_stat;
    std::string send_content;
    std::cout << url << std::endl;
    int res = stat(url.data(),&m_real_stat);
    // int fd = open(url.data(),O_RDONLY);
    // if(stat(url.data(),&m_real_stat) < 0){
    //     reac_http_body += "404 ";
    //     return NO_RESOURCE;
    // }
    // if(!(m_real_stat.st_mode & S_IROTH)){
    //     return FOBIDDEN_REQUEST;
    // }
    // if(S_ISDIR(m_real_stat.st_mode)){
    //     return BAD_REQUEST;
    // }
    if(res < 0){
        code = " 400 no resouce\r\n";
    }else{
        
        code = " 200 nice\r\n";
     
        if( !(m_real_stat.st_mode & S_IROTH)){
            code = " 400 fobidden\r\n";
        }
        if(S_ISDIR( m_real_stat.st_mode )){
            code = " 400 is dir\r\n";
        }
        
        
    }
    // std::cout << code << std::endl;
    reac_http_body = "";
    reac_http_body += http_message["version"];
    
    reac_http_body += code;
    
    reac_http_body += "Content-Type: text/html\r\n";
    reac_http_body += "Server: jojo/text\r\n";
    reac_http_body += "Connection: ";
    reac_http_body += http_message["Connection"];
    reac_http_body += "\r\n";

    if( res == 0 && code == " 200 nice\r\n"){
        std::cout << "打开文件" << std::endl;
        int fd = open(url.data(),O_RDONLY);
        share_addr = (char *)mmap(NULL,m_real_stat.st_size,PROT_READ,MAP_PRIVATE,fd,0);
        reac_http_body += "Content-Length: ";
        reac_http_body += std::to_string(m_real_stat.st_size);
        reac_http_body += "\r\n\r\n";
        reac_http_body += share_addr;

        
        munmap(share_addr,m_real_stat.st_size);
        close(fd);
        return true;
    }else{
        reac_http_body += "Content-Length: ";
        reac_http_body += "0";
        reac_http_body += "\r\n\r\n";
        return false;
    }
    
    
    
    
    
   
    // iov[0].iov_base = (void *)reac_http_body.data();
    // iov[0].iov_len = reac_http_body.size();
    // iov[1].iov_base = share_addr;
    // iov[1].iov_len = m_real_stat.st_size;
    // byte_to_send = reac_http_body.size() + m_real_stat.st_size;
    
    

}
bool func_get(){
    return true;
}
void func_post(){

}
Work::Work(int fd,struct sockaddr_in from_addr,int epollnum){
    std::cout << "新建一哈" << std::endl;
    work_on_fd = fd;
    readbuff = 0;
    work_type = "non";
    work_from_addr = from_addr;
    resourc = "../resource";
    epoll_on_fd = epollnum;
    reac_http_body = "";
    //将要发送的字节数
    byte_to_send = 0;
    //已经发送的字节数
    byte_have_send = 0;
    is_conn = true;
    already_write = 0;
}
Work::~Work(){
    
}

void Work::set_work_type(std::string w_type){
    work_type = w_type;
}

Work::HTTP_CODE Work::parse_line(std::string str){
    int cut_index = 0;
    // std::cout << readbuff << std::endl;
    // std::cout << str << std::endl;
    for(int i = 0;i < readbuff;i++){
        if (*(str.cbegin()+i) == '\r')
        {
            if((i+1) == readbuff && *(str.cbegin()+i+1) != '\n'){
                // std::cout << "到这了1" << std::endl;
                // std::cout << *(str.cbegin()+i+1) << std::endl;

                return BAD_REQUEST;
            }
            else if(*(str.cbegin()+i+1) == '\n'){
                // std::cout << "到这了3" << std::endl;
                if(cut_index == i && cut_index!=0){
                    i++;
                    cut_index = i+1;
                    line.push_back("");
                    if(cut_index < readbuff){
                        // std::cout << "到这了4" << std::endl;
                        line.push_back(str.substr(cut_index,readbuff));
                        return GET_REQUEST;
                    }
                }else{
                    // std::cout << "到这了2" << std::endl;
                    line.push_back(str.substr(cut_index,i));
                    i++;
                    cut_index = i+1;
                }
                
            }else{
                return BAD_REQUEST;
            }
        }
        else if(*(str.cbegin()+i) == '\n'){
            if(i > 1 && (*(str.cbegin()+i-1) == '\r')){
                cut_index = 2;
            }
            
        }else{

        }
    }
    return GET_REQUEST;
}

void Work::clear_the_data(){
    http_message.clear();
    line.clear();
    memset(http_data,'\0',sizeof(http_data));
    readbuff = 0;
    reac_http_body = "";
    
    //将要发送的字节数
    byte_to_send = 0;
    //已经发送的字节数
    byte_have_send = 0;
    already_write = 0;
    
}
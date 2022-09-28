#include "httpconn.h"

int httpconn::epoll_fd = -1;
int httpconn::user_count = 0;
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

void httpconn::init(int fd,sockaddr_in & addr){
    sock_fd = fd;
    m_sockaddr = addr;
    int optv = 1;
    setsockopt(sock_fd,SOL_SOCKET,SO_REUSEPORT,&optv,4);
    user_count += 1;
    epoll_add(epoll_fd,fd,true);
    init();
}

void httpconn::init(){
    main_state = CHECK_STATE_REQUESTLINE;
    m_checked_idx = 0;
    m_start_line = 0;
    m_read_line = 0;
    m_method = GET;
    m_url = 0;
    m_version = 0;
    m_linker = false;
    bzero(m_read_buf,READ_BUF_SIZE);
    m_conten_lenth = 0;
}
void httpconn::close_conn(){
    
    epoll_del(epoll_fd,sock_fd);
    user_count--;
    sock_fd = -1;

}

bool httpconn::read(){
    if(read_index >= READ_BUF_SIZE){
        return false;
    }
    int bytes_read = 0;
    while (true)
    {
        bytes_read = recv(sock_fd,m_read_buf + read_index,READ_BUF_SIZE - read_index,0);
        if(bytes_read == -1){
            if(errno == EAGAIN || errno == EWOULDBLOCK){
                break;
            }
            return false;
        }
        else if (bytes_read == 0)
        {
            return false;
        }
        read_index += bytes_read;
        
    }
    
    return true;
}

httpconn::HTTP_CODE httpconn::procss_read(){
    CHECK_LINE line_stat = LINE_OK;
    HTTP_CODE ret = NO_REQUEST;
    char * text = 0;
    std::cout << read_index << std::endl;
    while(((main_state == CHECH_STATE_CONTENT) && line_stat == LINE_OK)||(line_stat=the_line()) == LINE_OK){
        text = getline();
        std::cout << "procss_read "<< std::endl;
        m_start_line = m_checked_idx;
        std::cout << "get one line:" << text << std::endl;
        switch (main_state)
        {
        case CHECK_STATE_REQUESTLINE:
        {
            ret = parse_request_line(text);
            if(ret == BAD_REQUEST){
                return BAD_REQUEST;
            }
            break;
        }
            
        case CHECK_STATE_HEADER:
        {
            ret = parse_headers(text);
            if(ret == BAD_REQUEST){
                return BAD_REQUEST;
            }else if(ret == GET_REQUEST){
                return do_request();
            }
            break;
        }
            
        case CHECH_STATE_CONTENT:
        {
            ret = parse_content(text);
            if(ret == GET_REQUEST){
                return do_request();
            }
            line_stat = LINE_OPEN;
            break;
        }
            
        default:
            return INTERNAL_ERROR; 
            break;
        }
    }
    return NO_REQUEST;
}
httpconn::HTTP_CODE httpconn::parse_request_line(char * text){
    std::cmatch match;
    std::regex patten("([[:print:]]+) ([[:print:]]+) ([[:print:]]+)");
    regex_search(text,match,patten);
    if(match.size() != 4){
        return BAD_REQUEST;
    }
    if (match.str(1) == "GET")
    {
        m_method = GET;
    }else if(match.str(1) == "POST"){
        m_method = POST;
    }else{
        return BAD_REQUEST;
    }

    
    m_version = match.str(3).data();
    m_url = match.str(2).data();
    patten = "http";
    regex_search(m_url,match,patten);
    if (match.size() > 0)
    {
       patten = "[[:print:]]+://([[:print:]]+\\d)([[:print:]]+)";
       regex_search(m_url,match,patten);
       m_url = match.str(2).data();
    }
    main_state = CHECK_STATE_HEADER;
    
    return NO_REQUEST;
}
httpconn::HTTP_CODE httpconn::parse_headers(char * text){
    if(text[0] == '\0'){
        if(m_conten_lenth != 0){
            main_state = CHECH_STATE_CONTENT;
            return NO_REQUEST;
        }
        return GET_REQUEST;
    }else{
        std::cmatch match;
        std::regex patten("([[:print:]]+): ([[:print:]]+)");
        regex_search(text,match,patten);
        if(match.size() < 3){
            return BAD_REQUEST;
        }else{
            if(match.str(1) == "Host"){
                m_host = match.str(2).data();
            }else if(match.str(1) == "Connection"){
                if(match.str(2) == "keep-alive"){
                    m_linker = true;
                }
                
            }else if(match.str(1) == "Conten-Length"){
                m_conten_lenth = stoi(match.str(2));
            }else{
                std::cout << "未知" << std::endl;
            }
        }
        return NO_REQUEST;
    }
    
    return NO_REQUEST;
}
httpconn::HTTP_CODE httpconn::parse_content(char * text){

    return NO_REQUEST;
}

httpconn::CHECK_LINE httpconn::the_line(){
    char temp;
    std::cout << "the_line "<< std::endl;
    for(; m_checked_idx < read_index ;m_checked_idx++){
        temp = m_read_buf[m_checked_idx];
        if (temp == '\r')
        {
            if((m_checked_idx+1) == read_index){
                std::cout << "LINE_OPEN "<< std::endl;
                return LINE_OPEN;
                
            }else if(m_read_buf[m_checked_idx+1] == '\n'){
                m_read_buf[m_checked_idx++] = '\0';
                m_read_buf[m_checked_idx++] = '\0';
                std::cout << "LINE_OK "<< std::endl;
                return LINE_OK;
            }else{
                std::cout << "LINE_BAD "<< std::endl;
                return LINE_BAD;
            }
        }else if(temp == '\n'){
            if(m_checked_idx > 1 && (m_read_buf[m_checked_idx-1] == '\r')){
                m_read_buf[m_checked_idx-1] = '\0';
                m_read_buf[m_checked_idx++] = '\0';
                std::cout << "LINE_OK "<< std::endl;
                return LINE_OK;
            }
            return LINE_BAD;
        }else{
            
        }
        
        
    }
    return LINE_BAD;
}
httpconn::HTTP_CODE httpconn::do_request(){
    m_real_file = doc_root;
    std::cout << "do_request "<< std::endl;
    m_real_file += m_url;
    if(stat(m_real_file.data(),&m_real_stat) < 0){
        return NO_RESOURCE;
    }
    if(!(m_real_stat.st_mode & S_IROTH)){
        return FOBIDDEN_REQUEST;
    }
    if(S_ISDIR(m_real_stat.st_mode)){
        return BAD_REQUEST;
    }
    int fd = open(m_real_file.data(),O_RDONLY);
    share_addr = (char *)mmap(NULL,m_real_stat.st_size,PROT_READ,MAP_PRIVATE,fd,0);
    
    close(fd);
    return FILE_REQUEST;

}

bool httpconn::write(){
    int temp;
    while (1)
    {
        temp = writev(sock_fd,iov,2);
        if(temp <= -1){
            if(errno == EAGAIN){
                epoll_change(epoll_fd,sock_fd,EPOLLOUT);
                return true;
            }
            munmap(share_addr,m_real_stat.st_size);
            return false;
        }
    }
    
    return true;
}

bool httpconn::procss_write(httpconn::HTTP_CODE react){
    std::cout << "write "<< std::endl;
    switch (react)
    {
    case INTERNAL_ERROR:
        return false;
        break;
    case BAD_REQUEST:
        return false;
        break;
    case NO_RESOURCE:
        return false;
        break;
    case FOBIDDEN_REQUEST:
        return false;
        break;
    case GET_REQUEST:
        ack_http = m_version;
        ack_http += " 200 OK\r\n";
        ack_http += "Connection: keep-alive\r\n";
        ack_http += "Content-Type: text/html;charset=utf-8\r\n";
        ack_http += "Server: jojo\r\n";
        ack_http += "Content-Type: text/html;charset=utf-8\r\n";
        ack_http += "\r\n";
        iov[0].iov_base = ack_http.data();
        iov[0].iov_len = ack_http.size();
        iov[1].iov_base = share_addr;
        iov[1].iov_len = m_real_stat.st_size;
        return true;
        break;
    default:
        return false;
        break;
    }
}

void httpconn::process(){
    std::cout << "proccess" << std::endl;
    HTTP_CODE reac = procss_read();
    if (reac == NO_REQUEST)
    {
        std::cout << "NO_REQUEST "<< std::endl;
        epoll_change(epoll_fd,sock_fd,EPOLLIN);
        return;
    }
    bool write_ref = procss_write(reac);
    if (!write_ref)
    {
        close_conn();
    }
    epoll_change(epoll_fd,sock_fd,EPOLLOUT);
    
}
#include "vector"
#include "iostream"
#include "../include/locker.h"
#include "regex"
#include "sys/stat.h"
int main(){
    char*  a = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:103.0) Gecko/20100101 Firefox/103.0";
    std::cmatch match;
    char *b;
    std::string out;
    out += " 200 OK\r\n";
    out += "Connection: keep-alive\r\n";
    out += "Content-Type: text/html;charset=utf-8\r\n";
    out += "Server: jojo\r\n";
    out += "Content-Type: text/html;charset=utf-8\r\n";
    out += "\r\n";
    
    std::cout << out<< std::endl;
    // std::regex patten("([[:print:]]+): ([[:print:]]+)");
    // // std::regex patten("([[:print:]]+) ([[:print:]]+) ([[:print:]]+)");
    // // strcmp("GET","get");
    // regex_search(a,match,patten);
    // for(int i = 0;i < match.size();i++){
    //     std::cout << match[i] << std::endl;
    // }
    


}
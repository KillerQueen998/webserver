#include "vector"
#include "string"
#include "memory"
#include "iostream"
#include "unistd.h"
#include "regex"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "sys/mman.h"
#include "vector"
using namespace std;
void quiksort(vector<int> list,int r,int l){
    int low = r;
    int high = l;
    int key = list[high];
    int temp = 0;
    while (low < high)
    {
        while(list[low] <= key){
            low++;
            if(low == high){
                break;
            }
        }
        while(list[high] >= key){
            high--;
            if(low == high){
                break;
            }
        }
        temp = list[high];
        list[high] = list[low];
        list[low] = temp;
    }
    quiksort(list,r,low-1);
    quiksort(list,low+1,l);

}
int main(){
    vector<int> a = {57, 68, 59, 52, 72, 28, 96, 33, 24};
    quiksort(a,0,a.size()-1);
    for (auto x:a)
    {
        std::cout << x << std::endl;
    }
    
    // struct stat m_real_stat;
    // size_t b = 100;
    // std::string a = "../resource/";
    // std::cout << stat(a.data(),&m_real_stat) << std::endl;
    // int x = open(a.data(),O_RDONLY);
    // char * share_addr = (char *)mmap(NULL,100,PROT_READ,MAP_PRIVATE,x,0);
    // std::cout << share_addr << std::endl;
    // std::smatch match;
    // std::regex patten("[[:print:]]+://([[:print:]]+\\d)([[:print:]]+)");
    // std::regex_search(a,match,patten);
    // for(int i = 0;i < match.size();i++){
    //     std::cout << match.str(i) << std::endl;
    // }
    // std::vector<std::string> x(100);
//     std::string data = R"s~s+s(GET / HTTP/1.1
// Host: localhost:19980
// User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:103.0) Gecko/20100101 Firefox/103.0
// Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8
// Accept-Language: zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2
// Accept-Encoding: gzip, deflate, br
// Connection: keep-alive
// Upgrade-Insecure-Requests: 1
// Sec-Fetch-Dest: document
// Sec-Fetch-Mode: navigate
// Sec-Fetch-Site: none
// Sec-Fetch-User: ?1
// )s~s+s";
//     std::vector<std::string> line;
//     // std::cout << data.substr(0,2) << std::endl ;
//     int cut_index = 0;
//     for(int i = 0;i < data.size();i++){
//         if (*(data.cbegin()+i) == '\r')
//         {
//             if((i+1) == data.size()){
//                 return false;
//             }
//             else if(*(data.cbegin()+i+1) == '\n'){
//                 if(cut_index == i && cut_index!=0){
//                     i++;
//                     cut_index = i+1;
//                     if(cut_index < data.size()){
//                         line.push_back(data.substr(cut_index,data.size()));
//                         return true;
//                     }
//                 }else{
//                     line.push_back(data.substr(cut_index,i));
//                     i++;
//                     cut_index = i+1;
//                 }
                
//             }else{
//                 return false;
//             }
//         }
//         else if(*(data.cbegin()+i) == '\n'){
//             if(i > 1 && (*(data.cbegin()+i-1) == '\r')){
//                 cut_index = 2;
//             }
            
//         }else{

//         }
//     }
//     return true;
//     std::cout << line.size() << std::endl;
//     std::regex patten("[[:print:]]+ [[:print:]]+\n");
//     std::smatch match;
//     regex_search(data,match,patten);
//     for(auto itr:match){
//         std::cout << itr << std::endl;
//     }
    
}
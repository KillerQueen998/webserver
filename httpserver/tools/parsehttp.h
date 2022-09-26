#ifndef PARSEHTTP_H
#define PARSEHTTP_H
#include "string"
#include "vector"
class parsehttp{
    public:
        enum METHOD{GET=0,POST,HEAD};
        enum CHECK_STATE{ CHECK_STATE_REQUESTLINE = 0,CHECK_STATE_HEADER,CHECH_STATE_CONTENT}; 
        enum CHECK_LINE {LINE_OK = 0,LINE_BAD,LINE_OPEN};
        enum HTTP_CODE {NO_REQUEST = 0,GET_REQUEST,BAD_REQUEST,NO_RESOURCE,FOBIDDEN_REQUEST,FILE_REQUEST,INTERNAL_ERROR,CLOSED_CONNECT};
        CHECK_LINE parse_line(std::string str);
        
    private:
        
    // std::cout << data.substr(0,2) << std::endl ;
    

    
};
parsehttp::CHECK_LINE parsehttp::parse_line(std::string str){
    int cut_index = 0;
    std::vector<std::string> line;
    for(int i = 0;i < str.size();i++){
        if (*(str.cbegin()+i) == '\r')
        {
            if((i+1) == str.size()){
                return false;
            }
            else if(*(str.cbegin()+i+1) == '\n'){
                if(cut_index == i && cut_index!=0){
                    i++;
                    cut_index = i+1;
                    if(cut_index < str.size()){
                        line.push_back(str.substr(cut_index,str.size()));
                        return true;
                    }
                }else{
                    line.push_back(str.substr(cut_index,i));
                    i++;
                    cut_index = i+1;
                }
                
            }else{
                return false;
            }
        }
        else if(*(data.cbegin()+i) == '\n'){
            if(i > 1 && (*(data.cbegin()+i-1) == '\r')){
                cut_index = 2;
            }
            
        }else{

        }
    }
    return true;
}


#endif
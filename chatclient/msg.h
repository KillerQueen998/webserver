#ifndef MSG_H
#define MSG_H
#include <QString>

class msg
{
private:
    QString data;
    //0读 1写
    int flag;
public:
    msg(int flag);
};

#endif // MSG_H

#include "mythread.h"

QMutex mythread::lock;
QSemaphore mythread::sem;
QSemaphore mythread::sem2;
mythread::mythread(int flag,QString ip,QString port,QTextBrowser *chattext,QTcpSocket* f_socket,QVector<QString> * list,QPushButton * qpbutton):is_stop(false),m_ip(ip),m_port(port)
{
    this->flag = flag;
    m_chattext = chattext;
    this->socket = f_socket;
    m_button = qpbutton;
    this->msg_list = list;
    connect(socket,&QTcpSocket::readyRead,this,[=](){
        sem2.release();
    });
    connect(m_button,&QPushButton::clicked,this,[=](){
        emit m_sig();
    });

}

mythread::~mythread(){
    m_chattext = nullptr;
    delete m_chattext;
    sem.release(sem.available());
    sem2.release(sem2.available());
    delete msg_list;
}

void mythread::run(){
    connect(this,&mythread::m_sig,this,[=](){
        qDebug() << "shoudaole";
        this->exit(0);
        qDebug() << "zuodaole";
    });
    if(flag == 0){
        while(!is_stop){
            sem2.acquire();
            auto data = socket->readAll();
            qDebug() << "收到了" + data;
            if(data.size() != 0){
                lock.lock();
                msg_list->push_front(data);
                sem.release();
                lock.unlock();
            }

        }
    }else{
        while(!is_stop){
            sem.acquire();

            if(msg_list->size() != 0){
                lock.lock();
                QString user_msg = msg_list->back();
                qDebug() <<  msg_list->size();
                msg_list->pop_back();
                lock.unlock();
                int num_ = user_msg.size()/20+1;

                QString output_ = "";

                if (num_ == 1){
                    output_ += user_msg;
                    output_ += "\n";
                }else {
                    for(int i = 0;i < num_;i++){
                       if(i * 20 > user_msg.size()){
                           output_ += "\n";
                           break;
                       }
                       output_ += user_msg.mid(i*20,20<(user_msg.size()-i*20)?20:-1);
                       output_ += "\n";
                    }
                }

                if(user_msg != ""){
                    m_chattext -> insertPlainText(output_);
                }

            }
        }

    }
//    sem.acquire();
//    this->socket = new QTcpSocket(this);
//    socket->connectToHost(QHostAddress("47.100.63.170"), 19980);

//    //当socket发送连接成功信号的时候，实现以下槽函数
//    connect(socket, &QTcpSocket::connected, this, [=]()
//    {
//        while(is_stop){
//            if(flag == 0){

//            }else{

//            }
//        }
//    });
//    //当socket发送断开连接信号的时候，实现以下槽函数
//    connect(socket, &QTcpSocket::disconnected, this, [=]()
//        {
//            socket->close();
//            m_chatPage->close();
//            QMessageBox::information(NULL, "提示", "连接断开",QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
//            socket->deleteLater();//释放指向的内存
//    });
//    //当socket发送有内容可读的信号的时候，实现以下槽函数（在该函数接受socket对象的内容）
//    connect(socket, &QTcpSocket::readyRead, this, [=]()
//        {
//            QString messageServer = socket->readAll();//接受服务器传来的内容
//            qDebug() << messageServer;

//            //将数据写到服务器中
//            socket->write("我是客户端");//socket的write方法其实是有几个重载方法的，如果为了方便，可以直接用QString类型的变量作为参数。
//        });








}

void mythread::StopThread()
{
    is_stop = true;
}
//void mythread::set_msg(std::shared_ptr<msg> msg){
//    m_msg = msg;
//}

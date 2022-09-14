//#include "socketchat.h"

//socketchat::socketchat(QString ip,QString port)
//{
//    lock = new QMutex();
//    sem = new QSemaphore();
//    this->ip = ip;
//    this->port = port;
//    for(int i = 0;i < 2;i++){

//        threadpool.push_back(std::make_shared<mythread>());

//    }
//    for(auto i :threadpool){
//        qDebug() << "线程启动了";
//        while(i->is_stop){
//            sem ->acquire();
//            lock->lock();
//            auto msgdata = msglist.back();
//            i->set_msg(msgdata);
//            msglist.pop_back();
//            lock->unlock();
//            i -> start();
//        }

//    }
//}

//void socketchat::add_msg(std::shared_ptr<msg> msg){
//    lock->lock();
//    msglist.push_back(msg);
//    sem->release();
//    lock->unlock();
//}

//void run_thread(){

//}

//socketchat::~socketchat(){
//    delete sem;
//    delete lock;
//}






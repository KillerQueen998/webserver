#include "chatpage.h"
#include "ui_chatpage.h"


chatPage::chatPage(QTcpSocket *socket,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::chatPage)
{
    ui->setupUi(this);
//    this->setAttribute(Qt::WA_QuitOnClose,true);
    this->socket = socket;
//    connect(socket, &QTcpSocket::disconnected, this, [=]()
//        {
//            socket->close();
//            this->close();
//            QMessageBox::information(NULL, "提示", "连接断开",QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
//            socket->deleteLater();//释放指向的内存
//        });
    msg_list = new QVector<QString>();
    thread_1 = new mythread(0,"test","test",this->ui->chattext,socket,msg_list,this->ui->returnbutton);
    thread_2 = new mythread(1,"test","test",this->ui->chattext,socket,msg_list,this->ui->returnbutton);

    thread_1->start();
    thread_2->start();

}

chatPage::~chatPage()
{
//    disconnect(socket, &QTcpSocket::disconnected, this,0);

    delete ui;



    thread_1->deleteLater();
    thread_2->deleteLater();
    socket = nullptr;
    delete socket;
    delete msg_list;
}

void chatPage::on_returnbutton_clicked()
{


    thread_1->is_stop = true;
    thread_2->is_stop = true;
//    mythread::sem2.release();
//    mythread::sem.release();
//    thread_1->quit();
//    thread_2->quit();
//    thread_1->wait();
//    thread_2->wait();
    thread_1->deleteLater();
    thread_2->deleteLater();
    this->close();
//    this->deleteLater();

}

Ui::chatPage * chatPage::getui(){
    return ui;
}

void chatPage::on_submit_clicked()
{
    user_msg = ui->text ->toPlainText();
    int num_ = user_msg.size()/20+1;

    QString output_ = "";
    QString user_ = "                                                                          ";
    if (num_ == 1){
        output_ += user_;
        output_ += user_msg;
        output_ += "\n";
    }else {
        for(int i = 0;i < num_;i++){
           if(i * 20 > user_msg.size()){
               output_ += "\n";
               break;
           }
           output_ += user_;
           output_ += user_msg.mid(i*20,20<(user_msg.size()-i*20)?20:-1);
           output_ += "\n";
        }
    }

    if(user_msg != ""){
        ui->chattext -> insertPlainText(output_);
        auto msg = user_msg.toStdString();
        socket->write(msg.data());
        ui->text->clear();
    }


}


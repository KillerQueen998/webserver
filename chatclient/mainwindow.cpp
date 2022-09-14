#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    this->setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);
    this->setAttribute(Qt::WA_QuitOnClose,true);
}

MainWindow::~MainWindow()
{
    delete ui;

}


void MainWindow::on_login_clicked()
{
    bool islogin = true;
    if(islogin){
        auto socket = new QTcpSocket(this);
        socket -> connectToHost(QHostAddress("47.100.63.170"), 19980);
        connect(socket,&QTcpSocket::errorOccurred ,this,[=](){
            socket->close();
            QMessageBox::information(NULL, "提示", "连接失败",QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
            disconnect(socket,&QTcpSocket::connected,this,0);
            disconnect(socket,&QTcpSocket::errorOccurred,this,0);

        });
        connect(socket, &QTcpSocket::connected, this, [=]()
        {
            this->hide();
            QMainWindow *a = new secondWindow(socket,this);
//            a->setAttribute(Qt::WA_DeleteOnClose);
            a->show();
            disconnect(socket,&QTcpSocket::connected,this,0);
            disconnect(socket,&QTcpSocket::errorOccurred,this,0);

        });

        socket = nullptr;
        delete socket;

    }

}








//void MainWindow::on_regis_clicked()
//{
//    QMessageBox::information(NULL, "提示", "连接失败",QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
//}


#include "secondwindow.h"
#include "ui_secondwindow.h"

secondWindow::secondWindow(QTcpSocket * m_socket, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::secondWindow)
{

    ui->setupUi(this);
    this->socket = m_socket;
//    this->setAttribute(Qt::WA_QuitOnClose,true);
    connect(socket, &QTcpSocket::disconnected, this, [=]()
        {
            disconnect(socket, &QTcpSocket::disconnected, this,0);
            m_parent = parent;

            QMessageBox::information(NULL, "提示", "连接断开",QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Ok);
            socket->close();
            socket->deleteLater();//释放指向的内存
            this->close();
            parent->show();

        });
}

secondWindow::~secondWindow()
{
    m_parent -> show();
    disconnect(socket, &QTcpSocket::disconnected, this,0);
    socket->close();
    socket->deleteLater();
    delete socket;
    delete ui;


}


void secondWindow::on_pushButton_clicked()
{
//    this->hide();
    auto p_chat = new chatPage(this->socket,this);
    p_chat->setAttribute(Qt::WA_DeleteOnClose);
    p_chat->show();
}


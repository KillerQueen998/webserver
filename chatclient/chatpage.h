#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <QWidget>
#include <socketchat.h>
#include "qtcpsocket.h"
#include "ui_chatpage.h"
#include <QMessageBox>
#include <mythread.h>
#include <vector>
namespace Ui {
class chatPage;
}

class chatPage : public QWidget
{
    Q_OBJECT

public:
    explicit chatPage(QTcpSocket* socket,QWidget *parent = nullptr);
    ~chatPage();
    Ui::chatPage * getui();

private slots:
    void on_returnbutton_clicked();

    void on_submit_clicked();

private:
    Ui::chatPage *ui;
    QTcpSocket* socket;
    QString user_msg;
    mythread *thread_1;
    mythread *thread_2;
    QVector<QString> * msg_list;
};

#endif // CHATPAGE_H

#ifndef MYTHREAD_H
#define MYTHREAD_H
class chatPage;
#include <qthread.h>
#include <qdebug.h>
#include <msg.h>
#include <memory>
#include <qtcpsocket.h>
#include <qhostaddress.h>
#include <QSemaphore>
#include <QMutex>
#include <QTextBrowser>
#include <QMessageBox>
#include <QPushButton>
class mythread : public QThread
{
    Q_OBJECT
public:
    mythread(int flag,QString ip,QString port,QTextBrowser * chattext,QTcpSocket* socket,QVector<QString> *list,QPushButton * qpbutton);
    ~mythread();
    virtual void run() override;
    void StopThread();
    bool is_stop;
    static QMutex lock;
    static QSemaphore sem;
    static QSemaphore sem2;
signals:
    void m_sig();
//    void set_msg(std::shared_ptr<msg> m_msg);
private:

//    std::shared_ptr<msg> m_msg;
    //0读 1展示
    int flag;

    QTcpSocket* socket;
    QString m_ip;
    QString m_port;
    QVector<QString> *msg_list;
    QTextBrowser * m_chattext;
    QPushButton * m_button;

};

#endif // MYTHREAD_H

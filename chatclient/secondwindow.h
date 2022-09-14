#ifndef SECONDWINDOW_H
#define SECONDWINDOW_H

#include "qtcpsocket.h"
#include <QMainWindow>
#include <chatpage.h>
namespace Ui {
class secondWindow;
}

class secondWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit secondWindow(QTcpSocket * m_socket,QWidget *parent = nullptr);
    ~secondWindow();

private slots:
    void on_pushButton_clicked();


private:
    Ui::secondWindow *ui;
    QTcpSocket *socket;
    QWidget * m_parent;
};

#endif // SECONDWINDOW_H

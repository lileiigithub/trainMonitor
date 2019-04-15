#ifndef LOGIN_H
#define LOGIN_H

#include <QWidget>
#include <qdebug.h>
#include <QMessageBox>
namespace Ui {
class Login;
}

class Login : public QWidget
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = 0);
    ~Login();

bool pwIsRight = false;

signals:
    void toHomePage();

private slots:
    void on_pushButton_ok_clicked();

    void on_pushButton_cancel_clicked();

private:
    Ui::Login *ui;

    QString user;
//    QString password;

};

#endif // LOGIN_H

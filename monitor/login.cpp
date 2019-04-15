#include "login.h"
#include "ui_login.h"
#include <QDir>
Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    this->setWindowTitle("登录界面");
    //设置快捷键
    ui->pushButton_ok->setShortcut(QKeySequence(tr("Enter"))); //QLatin1String

    //<设置背景图片
    this->setAutoFillBackground(true);
    QPalette palette;
    palette.setBrush(QPalette::Background, QBrush(QPixmap("../monitor/background/login.jpg"))); //
    this->setPalette(palette);
    QDir dir("./monitor/background/login.jpg");
    qDebug()<<QDir::currentPath()<<"-------"<<dir.exists();
    //>
    //设置字体
    ui->lineEdit_user->setFont(QFont("AnyStyle",14,QFont::Normal));
    ui->lineEdit_password->setFont(QFont("AnyStyle",14,QFont::Normal));
    ui->lineEdit_password->setEchoMode(QLineEdit::Password);
}

Login::~Login()
{
    delete ui;
}

void Login::on_pushButton_ok_clicked()
{
    QString password;
    this->user = this->ui->lineEdit_user->text();
    password = this->ui->lineEdit_password->text();
    if(password == "123")
    this->pwIsRight = true;
    qDebug()<<"PASSWORD is "<<pwIsRight;
    if(pwIsRight == true){
        emit toHomePage();
        this->close();
    }
    else{
        QMessageBox::warning(this,tr("Warning"),("user name or password wrong!"),QMessageBox::Yes);
    }
}

void Login::on_pushButton_cancel_clicked()
{
    ui->lineEdit_password->clear();
}









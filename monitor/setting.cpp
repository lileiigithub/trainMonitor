#include "setting.h"
#include "ui_setting.h"

Setting::Setting(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Setting)
{
    ui->setupUi(this);
    //<设置返回按钮图标
    QIcon back_icon;
    back_icon.addFile("../monitor/icon/back_16x16.png");
    ui->pushButton_back->setIcon(back_icon); //>
}

Setting::~Setting()
{
    delete ui;
}

void Setting::on_pushButton_back_clicked()
{
    emit toHomePage();
    this->close();
}

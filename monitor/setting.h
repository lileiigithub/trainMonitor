#ifndef SETTING_H
#define SETTING_H

#include <QMainWindow>

namespace Ui {
class Setting;
}

class Setting : public QMainWindow
{
    Q_OBJECT

public:
    explicit Setting(QWidget *parent = 0);
    ~Setting();

signals:
    void toHomePage();

private slots:
    void on_pushButton_back_clicked();

private:
    Ui::Setting *ui;
};

#endif // SETTING_H




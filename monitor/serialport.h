#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QDebug>
#include <QString>
#include <QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QString>
#include <QQueue>
#include <QTime>
#include <QDate>
#include <QTimer>

#include <datatype.h>


namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();
    QString getBaud();
    QString getPort();
    QString getState();

    trainEnvironment displayDbData;
    bool dataIsRight=false;
    bool dataIsReady=false;

signals:
    void comlpeteDataIsReady();

private slots:
    void on_buttonBox_accepted();
    void update_processed_data();
    void read_raw_data_from_serialPort();
    void on_pushButton_update_clicked();
    void on_pushButton_close_clicked();

private:
    Ui::Dialog *ui;
    QQueue<QString> data_queue; //将满足格式要求的数据存储在、队列中，用于实时显示的队列
    QTimer *datatimer;
    QByteArray requestData; // 从串口接收到的数据，改数据并不满足定义的格式要求

    QSerialPort *my_serialport;
    void check_existing_serialport_and_update();
    int put_raw_data_to_queue(QByteArray data);
    int process_data_in_queue(void); //处理data_queue里的数据

};


#endif // DIALOG_H






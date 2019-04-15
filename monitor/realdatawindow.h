#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "qcustomplot.h"
#include <QMainWindow>
#include <QTimer>
#include <QTime>
#include <QVector>
#include <QtSerialPort/QSerialPort>
#include "serialport.h"
#include <QStandardItem>
#include <QStandardItemModel>
#include <datatype.h>

#include <QSqlDatabase>
#include <QSqlQuery>
#include "databaseconnection.h"
extern QSqlDatabase db;
enum environment_grade{perfect,good,medium,terrible};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0); //explicit:ban conversion of parameter
    ~MainWindow();
    void RealtimeGraphInit(QCustomPlot *customPlot); //设置画图属性
    void lcdshow();

    static QQueue<trainEnvironment> data_queue_db;
    //将用于显示的数据跟新
    QQueue<trainEnvironment> queue_graph_table_data;  //存储用于显示图形的数据

    void realtimeData(); //实时数据槽

//    typedef struct{
//        float temperature;
//        float humidity;
//        float PM10;
//        float CO2;
//    } tpValue;
    QVector<trainEnvironment> old_data_vector; //存储历史数据用于过滤异常值

    int old_data_nums=5; //存储历史数据的个数

signals:
    void toHomePage();

private slots:

    void on_BoxTem_clicked();
    void on_BoxHum_clicked();
    void on_BoxPM_clicked();
    void on_BoxCO2_clicked();
    void on_ButtonStart_clicked();
    void on_ButtonPause_clicked();
    void receiveBackAction();
    void on_checkBox_clicked();
    void on_ButtonBack_clicked();


    void serialPort();

    void gradeTimerSlot();

    void updateDataFromSp();


private:
    Ui::MainWindow *ui;
    QTimer dataTimer;   //用于定时绘图的定时器
    QTime timing; //用于接收数据计时
    int pauseCount; //用于暂停计时
    bool BoxTem = true;
    bool BoxHum = true;
    bool BoxPM = true;
    bool BoxCO2 = true;
    bool BoxLegend = true;
    //串口通信

    Dialog *SPdialog = new Dialog;
    int buad;
    QString port;
    bool turnOn = false;
    QSerialPort *my_serialport;
    QByteArray requestData;
    //
    QStandardItemModel dataModel;
    //用于显示一行数据的QStandardItem
    QStandardItem *item0;
    QStandardItem *item1;
    QStandardItem *item2;
    QStandardItem *item3;
    QStandardItem *item4;
    QStandardItem *item5;
    //Qprocessbar
    QTimer gradeTimer; //定时对环境进行评价
    environment_grade grade=good;

};

#endif // MAINWINDOW_H







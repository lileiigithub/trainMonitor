#ifndef ANALYSIS_H
#define ANALYSIS_H

#include <QMainWindow>
#include <QDebug>
#include <QTime>
#include <QDate>
#include <QQueue>
#include <datatype.h>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "databaseconnection.h"
//写文件
#include <QTextStream>
#include <QFile>
#include <QString>
#include <QDebug>
//python
#include <QProcess>
//绘图
#include <QPixmap>
extern QSqlDatabase db;

namespace Ui {
class Analysis;
}

class Analysis : public QMainWindow
{
    Q_OBJECT

public:
    explicit Analysis(QWidget *parent = 0);
    ~Analysis();

signals:
    void toHomePage();

private slots:
    void on_pushButton_back_clicked();
    void on_pushButton_search_clicked();

private:
    Ui::Analysis *ui;
    //数据查询
    QTime time_start_search,time_end_search;
    QDate date_search;
    int row_data_start=0;
    int row_data_end=0;

    QQueue<trainEnvironment> queue_record_data;

    void queryData(QDate date_search,QTime time_start_search,QTime time_end_search);
    void writeDataToText();
    void callPyCreatePicture();
    void loadPicture();
    void readResultFromTxt();
};

#endif // ANALYSIS_H

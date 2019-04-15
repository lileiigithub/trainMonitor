#ifndef QUERY_H
#define QUERY_H

#include <QMainWindow>
#include <qsql.h>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <realdatawindow.h> //引入数据存入数据库
#include <QTimer>
#include <QTime>
#include <QDate>
#include <QQueue>
#include "qcustomplot.h"
#include <QSqlQuery>
#include <QSqlQueryModel>
#include "databaseconnection.h"

extern QSqlDatabase db;

namespace Ui {
class Query;
}

class Query : public QMainWindow
{
    Q_OBJECT

public:
    explicit Query(QWidget *parent = 0);
    ~Query();

signals:
    void toHomePage();
private slots:
    void on_pushButton_back_clicked();
    void addDataToDb();
    void on_pushButton_search_clicked();
    void recordDataSlot();    //记录数据槽
    void on_BoxTem_clicked();
    void on_BoxHum_clicked();
    void on_BoxPM_clicked();
    void on_BoxCO2_clicked();
    void on_ButtonPause_clicked();
    void on_checkBox_clicked();

private:
    Ui::Query *ui;
    //创建数据库连接及数据库tableModel

    QSqlTableModel *tableModel; //数据库表格
    QSqlQueryModel *queryModel;
    bool dbConnectSuccess = false;
    bool dbCreateTableModelSuccess = false;

    //数据查询
    QTime time_start_search,time_end_search;
    QDate date_search;
    int row_data_start=0;
    int row_data_end=0;
    bool connectDatebase(void); //连接数据库，成功返回true，失败返回false
    bool createTableModel(void);
    void queryData(QDate date_search,QTime time_start_search,QTime time_end_search);
    void viewSearchData(void);

    //绘图
    QQueue<trainEnvironment> queue_record_data;

    trainEnvironment environment;
    QTimer dataTimer;
    bool BoxTem = true;
    bool BoxHum = true;
    bool BoxPM = true;
    bool BoxCO2 = true;
    bool BoxLegend = true;
    void customplotInit(QCustomPlot *customPlot);

};

#endif // QUERY_H






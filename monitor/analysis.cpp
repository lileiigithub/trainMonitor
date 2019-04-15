#include "analysis.h"
#include "ui_analysis.h"
#include <QTime>
#include <QDir>
Analysis::Analysis(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Analysis)
{
    ui->setupUi(this);
    //<设置返回按钮图标
    QIcon back_icon;
    back_icon.addFile("../monitor/icon/back_16x16.png");
    ui->pushButton_back->setIcon(back_icon); //>

    //初始化timeEdit,dateEdit
    ui->dateEdit->setDate(QDate::currentDate());
    ui->timeEdit_end->setTime(QTime(23,59,59));
    // 设置显示格式
    ui->timeEdit_start->setDisplayFormat("HH:mm:ss");
    ui->timeEdit_end->setDisplayFormat("HH:mm:ss");

    ui->textBrowser->setFont(QFont("AnyStyle",14,QFont::Normal));

}

Analysis::~Analysis()
{
    delete ui;
}

void Analysis::on_pushButton_back_clicked()
{
    emit toHomePage();
    this->close();
}

void Analysis::queryData(QDate date_search,QTime time_start_search,QTime time_end_search){
    trainEnvironment environment;
    //执行SQL查询语句
    QString SQLstatement;
    SQLstatement =
    "SELECT * FROM train WHERE time BETWEEN '"+time_start_search.toString()+"' AND '"+time_end_search.toString()+"'"
            +" AND "+"date='"+date_search.toString("yyyy-MM-dd")+"'";
    qDebug()<<"SQLstatement:\n"<<SQLstatement;
    QSqlQuery query(SQLstatement,db);

    while (query.next()) {
        //解析从数据库查询出来的数据，存入queue_record_data中，用于显示
        environment.date = query.value(0).toDate();
        environment.time = query.value(1).toTime();
        environment.temperature = query.value(2).toFloat();
        environment.humidity = query.value(3).toFloat();
        environment.PM10 = query.value(4).toFloat();
        environment.CO2 = query.value(5).toFloat();
        queue_record_data.enqueue(environment);
    }
}

void Analysis:: writeDataToText(){
    trainEnvironment tempdata;
    QFile file("../monitor/newData.txt");
    if(!file.open(QIODevice::WriteOnly|QIODevice::Text)) return; //文本只写
    QTextStream in(&file);

    for(int i=0; i<queue_record_data.length(); i++){
        tempdata = queue_record_data.at(i);
        in<<tempdata.temperature<<","<<tempdata.humidity<<","<<tempdata.PM10<<","<<tempdata.CO2<<"\n";
    }

    file.close();

}

void Analysis:: callPyCreatePicture(){
    QProcess *myProcess = new QProcess();
    QString path=QDir::currentPath();
    qDebug()<<"当前路径是:  "<<path<<endl;
    myProcess->start("cmd.exe"); //python D:/codeshang/tainEnvMonitor1/monitor/dbscan.py
    myProcess->write("python I:/Qt/tainEnvMonitor1/monitor/dbscan.py\r\n");
    myProcess->write("exit\r\n");
    myProcess->waitForStarted();
    myProcess->waitForFinished();
     QString strResult = QString::fromLocal8Bit(myProcess->readAllStandardOutput());
     qDebug()<<strResult;
    //    QTime t; t.start(); while(t.elapsed()<20000);
//    while (!myProcess->waitForFinished(30000))
//    {
//       qDebug()<<"结束失败\n";
//       ui->textBrowser->append("python程序结束失败！\n");
//    }
    qDebug()<<"python program finished !!";
}
void Analysis:: loadPicture(){
    QPixmap p;
    if(p.load("../monitor/pic.png")==false){
        ui->textBrowser->append("还没有生成图片！！\n");
    }
    else{
        ui->textBrowser->append("已生成图片！！\n");
        ui->label_picture->setScaledContents(true);
        ui->label_picture->setPixmap(p);
    }
}
void Analysis:: readResultFromTxt(){
    QFile file("../monitor/result.txt");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))return; //文本只写
    QTextStream out(&file);
    QString text = out.readAll();
    ui->textBrowser->append(text);
    file.close();
}
//获取查询的条件，并开始查询
void Analysis::on_pushButton_search_clicked()
{
    ui->textBrowser->append("正在计算,请等待...");
    ui->textBrowser->clear();
    time_start_search = ui->timeEdit_start->time();
    time_end_search = ui->timeEdit_end->time();
    date_search = ui->dateEdit->date();

    queue_record_data.clear(); //查询前清除数据
    queryData(date_search,time_start_search,time_end_search);
    if(queue_record_data.isEmpty()==false){
        ui->textBrowser->append(QString("查询到的数据条目：%1").arg(queue_record_data.length()));
        QTime t;
        t.start();
        while(t.elapsed()<1000);
         writeDataToText();
         callPyCreatePicture();
         qDebug()<<"begin load picture!!\n";
         loadPicture();
         readResultFromTxt();
    }
    else ui->textBrowser->append("查询数据为空，不能绘图！\n");
}




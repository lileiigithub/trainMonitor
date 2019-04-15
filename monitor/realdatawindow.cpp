#include "realdatawindow.h"
#include "ui_mainwindow.h"
#include <QMenu>
#include <random>
#include <qdebug.h>
#include <QString>
#include <QtMath>
#include "databaseconnection.h"

#define INTERVAL 1000 //time interval --ms
#define VIEWNUM 20 //tableview显示的数据条目
#define GRADE_TIME 10000 //评价环境的间隔时间, GRADE_TIME/1000 秒
QQueue<trainEnvironment> MainWindow::data_queue_db; //初始化静态成员，必须在.cpp文件中初始化
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Environment Monitor");
//    this->setFixedSize(1366,768);
    //设置窗口图标
    QIcon winIcon;
    winIcon.addFile("./icon/train.jpg");
    this->setWindowIcon(winIcon);
    //<设置返回按钮图标
    QIcon back_icon;
    back_icon.addFile("../monitor/icon/back_16x16.png");
    ui->ButtonBack->setIcon(back_icon); //>
    //画图
    RealtimeGraphInit(ui->qcustom);
    ui->qcustom->replot();
    //串口
    connect(ui->actionSerialPort, SIGNAL(triggered()), this, SLOT(serialPort()));
    connect(ui->actionBack,SIGNAL(triggered()),this,SLOT(receiveBackAction()));
    //初始化dataModel
    dataModel.setColumnCount(5); //设置列数为6
    dataModel.setHeaderData(0, Qt::Horizontal, tr("data"));
    dataModel.setHeaderData(1, Qt::Horizontal, tr("time"));
    dataModel.setHeaderData(2, Qt::Horizontal, tr("temperature"));
    dataModel.setHeaderData(3, Qt::Horizontal, tr("humidity"));
//    dataModel.setHeaderData(4, Qt::Horizontal, tr("PM10"));
    dataModel.setHeaderData(4, Qt::Horizontal, tr("CO2"));
    //设置tableView_realtime
    ui->tableView_realtime->setEditTriggers(QAbstractItemView::NoEditTriggers); //不能编辑表格
    //初始化Qprocessbar
    ui->progressBar->setValue(0);

    //初始化Vector
    for(int i = 0;i<old_data_nums;i++){
        trainEnvironment tempte;
        tempte.temperature=20;
        tempte.humidity=50;
        tempte.PM10=5;
        tempte.CO2=500;
        old_data_vector.append(tempte);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::RealtimeGraphInit(QCustomPlot *customPlot){
    customPlot->addGraph(); //
    customPlot->graph(0)->setPen(QPen(Qt::red));
    customPlot->graph(0)->setScatterStyle(QCPScatterStyle::ssDisc);
    customPlot->graph(0)->setName("温度");

    customPlot->addGraph(); //
    customPlot->graph(1)->setPen(QPen(Qt::blue));
//    customPlot->graph(1)->setLineStyle(QCPGraph::lsStepLeft);//lsLine
    customPlot->graph(1)->setScatterStyle(QCPScatterStyle::ssDisc);
    customPlot->graph(1)->setName("湿度");

    customPlot->addGraph(); //
    customPlot->graph(2)->setPen(QPen(Qt::green));
    customPlot->graph(2)->setScatterStyle(QCPScatterStyle::ssDisc);
    customPlot->graph(2)->setName("CO2");

//    customPlot->addGraph(); //
//    customPlot->graph(3)->setPen(QPen(Qt::black));
//    customPlot->graph(3)->setScatterStyle(QCPScatterStyle::ssDisc);
//    customPlot->graph(3)->setName("PM2.5");//颗粒物

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");//\n%z
    customPlot->xAxis->setTicker(timeTicker);
    customPlot->axisRect()->setupFullAxesBox();
    // make left and bottom axes transfer their ranges to right and top axes:
    connect(customPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->xAxis2, SLOT(setRange(QCPRange)));
    connect(customPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), customPlot->yAxis2, SLOT(setRange(QCPRange)));
 //   customPlot->yAxis->setRange(0, 100);
    customPlot->legend->setVisible(true);
    customPlot->legend->setBrush(QBrush(QColor(255,255,255,150)));
    customPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft|Qt::AlignTop);

    dataTimer.start(INTERVAL); // 1s interval to restart
    this->dataTimer.stop();
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(updateDataFromSp()));

    gradeTimer.start(GRADE_TIME);
    this->gradeTimer.stop();
    connect(&gradeTimer, SIGNAL(timeout()), this, SLOT(gradeTimerSlot()));
}

void MainWindow::realtimeData(){ //定时将数据刷新

    if(queue_graph_table_data.isEmpty() == false){
        trainEnvironment te,copyTe;
        te = queue_graph_table_data.dequeue();
        te.time=QTime::currentTime();
        //送入数据库的数据
        data_queue_db.enqueue(te);
        double key; // time elapsed since start of demo, in seconds
        // 拷贝串口数据到显示
        copyTe = te;
        /***************处理原始数据，去除异常值>>>***************/

        if(qFabs(copyTe.temperature-old_data_vector[0].temperature)>20 ||
                qFabs(copyTe.humidity-old_data_vector[0].humidity)>30  ||
                qFabs(copyTe.PM10-old_data_vector[0].PM10)>70  ||
                qFabs(copyTe.CO2-old_data_vector[0].CO2)>400  ){

           // 判定为异常值,取头值
           copyTe.temperature = old_data_vector[0].temperature;
           copyTe.humidity = old_data_vector[0].humidity;
           copyTe.PM10 = old_data_vector[0].PM10;
           copyTe.CO2 = old_data_vector[0].CO2;

        }
        else{ //不异常
            old_data_vector.pop_front();
            old_data_vector.append(copyTe);
        }
        qDebug()<<"队列头数据: "<<old_data_vector[0].temperature<<" "<<old_data_vector[0].humidity<<" "<<old_data_vector[0].CO2<<endl;
       /***************<<<<<处理原始数据，去除异常值***************/

        //QTime time;
        //time = QTime(SPdialog->displayDbData.time.split(":").at(0).toInt(),SPdialog->displayDbData.time.split(":").at(1).toInt(),SPdialog->displayDbData.time.split(":").at(2).toInt());
        key = (te.time.hour()*3600+te.time.minute()*60+te.time.second());//*100+QTime::currentTime().msec()

        // add data to lines:
        if(BoxTem == true) ui->qcustom->graph(0)->setVisible(true);
        else ui->qcustom->graph(0)->setVisible(false);
        if(BoxHum == true) ui->qcustom->graph(1)->setVisible(true);
        else ui->qcustom->graph(1)->setVisible(false);
        if(BoxCO2 == true) ui->qcustom->graph(2)->setVisible(true);
        else ui->qcustom->graph(2)->setVisible(false);
//        if(BoxPM == true) ui->qcustom->graph(3)->setVisible(true);
//        else ui->qcustom->graph(2)->setVisible(false);
        if(BoxLegend == true) ui->qcustom->legend->setVisible(true);
        else ui->qcustom->legend->setVisible(false);

        ui->qcustom->graph(0)->addData(key,copyTe.temperature );
        ui->qcustom->graph(1)->addData(key,copyTe.humidity );
        ui->qcustom->graph(2)->addData(key,copyTe.CO2 );
//        ui->qcustom->graph(3)->addData(key,copyTe.PM10 );

        // rescale value (vertical) axis to fit the current data:
        ui->qcustom->graph(0)->rescaleValueAxis(true);
        ui->qcustom->graph(1)->rescaleValueAxis(true);
        ui->qcustom->graph(2)->rescaleValueAxis(true);
//        ui->qcustom->graph(3)->rescaleValueAxis(true);
        // make key axis range scroll with the data (at a constant range size of 8):
        ui->qcustom->xAxis->setRange(key, 60, Qt::AlignRight);
        //lcd show
        lcdshow();
        ui->statusBar->showMessage(tr("State:%1  Port:%2  BuadRate:%3")
          .arg(SPdialog->getState()).arg(SPdialog->getPort()).arg(SPdialog->getBaud()));//("串口：com1")

        /*****************************tableview显示********************************/

        QList<QStandardItem*> list_item;
        item0 = new QStandardItem(te.date.toString("yyyy-MM-dd"));
        item1 = new QStandardItem(te.time.toString());
        item2 = new QStandardItem(QString("%1").arg(te.temperature));
        item3 = new QStandardItem(QString("%1").arg(te.humidity));
//        item4 = new QStandardItem(QString("%1").arg(te.PM10));
        item4 = new QStandardItem(QString("%1").arg(te.CO2));
        //水平垂直方向居中，写在new QStandardItem后,未生成QStandardItem对象前不能调用类函数
        item0->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        item1->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        item2->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        item3->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
//        item4->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        item4->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
        list_item.append(item0);
        list_item.append(item1);
        list_item.append(item2);
        list_item.append(item3);
        list_item.append(item4);
//        list_item.append(item5);
        dataModel.appendRow(list_item);
        ui->tableView_realtime->setModel(&dataModel);
        ui->tableView_realtime->resizeColumnsToContents(); //列宽适应表格
        ui->tableView_realtime->show();
        if(dataModel.rowCount()>VIEWNUM)dataModel.removeRow(0); //removeRow是否将数据冲堆里释放？？
        ui->qcustom->replot();
    }
}
/*
* 定时计算环境等级
*/
void MainWindow::gradeTimerSlot(){
    //计算等级
    double gradeScore=0;
    const double optimal_temperature=22,optimal_humidity=50,optimal_PM10=0,optimal_CO2=40;
    QQueue<trainEnvironment> queue_grade_data;
    trainEnvironment environment_grade;
    float mean_temperature=0,mean_humidity=0,mean_PM10=0,mean_CO2=0;
    QString SQLstatement;
    QTime time_start,time_end;
    QDate date=QDate::currentDate();
    qDebug()<<"gradeTimerSlot is run...";
    time_end = QTime::currentTime();
    time_start=time_end.addSecs((GRADE_TIME/1000)*(-1)); //查询开始时间
    //QSqlDatabase db_grade = QSqlDatabase::addDatabase("QMYSQL","localhost_3306");
    SQLstatement =
    "SELECT * FROM train WHERE time BETWEEN '"+time_start.toString()+"' AND '"+time_end.toString()+"'"
            +" AND "+"date='"+date.toString("yyyy-MM-dd")+"'";
    qDebug()<<"grade_data SQLstatement:"<<SQLstatement;
    QSqlQuery query(SQLstatement,db);

    while (query.next()) {
        //解析从数据库查询出来的数据，存入queue_record_data中，用于显示
        environment_grade.date = query.value(0).toDate();
        environment_grade.time = query.value(1).toTime();
        environment_grade.temperature = query.value(2).toFloat();
        environment_grade.humidity = query.value(3).toFloat();
        environment_grade.PM10 = query.value(4).toFloat();
        environment_grade.CO2 = query.value(5).toFloat();
        queue_grade_data.enqueue(environment_grade);
    }

    qDebug()<<"queue_grade_data length :"<<queue_grade_data.length();
    //求均值
    for(int i=0;i<queue_grade_data.length();i++)
    {
        trainEnvironment temp;
        temp = queue_grade_data.at(i);
        mean_temperature += temp.temperature;
        mean_humidity += temp.humidity;
        mean_CO2 += temp.CO2;
    }
    mean_temperature = mean_temperature/queue_grade_data.length();
    mean_humidity = mean_humidity/queue_grade_data.length();
    mean_CO2 = mean_CO2/queue_grade_data.length();
    qDebug()<<"mean_temperature:"<<mean_temperature;
    qDebug()<<"mean_humidity:"<<mean_humidity;
    qDebug()<<"mean_CO2:"<<mean_CO2;
    //计算等级
    gradeScore = qPow((mean_temperature-optimal_temperature)/20,2)+qPow((mean_humidity-optimal_humidity)/90,2)+qPow(qFabs(mean_CO2-optimal_CO2)/90,2)+qPow((mean_PM10-optimal_PM10)/90,2);
    qDebug()<<"gradeScore:"<<gradeScore;
    if(gradeScore<(4.5)){ //0.75*/4
        this->grade = perfect;
    }
    else if(gradeScore<(5.5)){//0.75*2/4
        this->grade = good;
    }
    else if(gradeScore<(6)){//0.75*3/4
        this->grade = medium;
    }
    else{
        this->grade = terrible;
    }

    switch(this->grade){
    case perfect:
        ui->progressBar->setStyleSheet(QString("QProgressBar::chunk{background-color: rgb(0, 255, 0)}"));//green
        ui->progressBar->setValue(100);
        break;
    case good:
        ui->progressBar->setStyleSheet(QString("QProgressBar::chunk{background-color: rgb(255, 255, 0)}"));//yellow
        ui->progressBar->setValue(75);
        break;
    case medium:
        ui->progressBar->setStyleSheet(QString("QProgressBar::chunk{background-color: rgb(255, 0, 0)}"));//red
        ui->progressBar->setValue(50);
        break;
    case terrible:
        ui->progressBar->setStyleSheet(QString("QProgressBar::chunk{background-color: rgb(0, 0, 0)}"));//black
        ui->progressBar->setValue(25);
        break;
    }
}


void MainWindow::lcdshow(){
    QTime time1(0,0,0);//没有static time会每次进入函数时生成一次
    // calculate two new data points:
    this->ui->lcdNumber->setDigitCount(10);
    this->ui->lcdNumber->display(time1.addSecs(timing.elapsed()/1000).toString());//
}
void MainWindow::on_BoxTem_clicked(){
    if(BoxTem == true) BoxTem= false;
    else BoxTem = true;
}

void MainWindow::on_BoxHum_clicked(){
    if(BoxHum == true) BoxHum= false;
    else BoxHum = true;
}

void MainWindow::on_BoxPM_clicked(){
    if(BoxPM == true) BoxPM= false;
    else BoxPM = true;
}

void MainWindow::on_BoxCO2_clicked(){
    if(BoxCO2 == true) BoxCO2= false;
    else BoxCO2 = true;
}

void MainWindow::on_ButtonStart_clicked(){
    this->dataTimer.start();
    timing.start();
    this->gradeTimer.start();
}

void MainWindow::on_ButtonPause_clicked()
{
    if(dataTimer.isActive()){
        this->dataTimer.stop();
        pauseCount = timing.elapsed();
        ui->ButtonStart->setEnabled(false);
    }
    else {
        this->dataTimer.start();
        pauseCount = timing.elapsed() - pauseCount;
        timing = (timing.addMSecs(pauseCount+500));// 对象timing = 新对象,+500以免时间突变
        ui->ButtonStart->setEnabled(true);
    }
    if(gradeTimer.isActive()){
        this->gradeTimer.stop();
    }
    else{
      this->gradeTimer.start();
    }
}

void MainWindow::serialPort(){
    SPdialog->show();
}

void MainWindow::receiveBackAction(){
    emit toHomePage();
    this->hide();
}

void MainWindow::on_checkBox_clicked()
{
    if(BoxLegend == true) BoxLegend= false;
    else BoxLegend = true;
}

void MainWindow::on_ButtonBack_clicked()
{
    emit toHomePage();
    this->hide();
}

//将在 serialport.h 中调用跟新用于显示的串口数据
void MainWindow::updateDataFromSp(){
    if(SPdialog->dataIsRight==true){
        queue_graph_table_data.enqueue(SPdialog->displayDbData);
        realtimeData();
    }
}





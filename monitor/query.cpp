#include "query.h"
#include "ui_query.h"

Query::Query(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Query)
{
    ui->setupUi(this);
    //<设置返回按钮图标
    QIcon back_icon;
    back_icon.addFile("../monitor/icon/back_16x16.png");
    ui->pushButton_back->setIcon(back_icon); //>

    //数据库配置
    connectDatebase(); //连接数据库
    createTableModel(); //创建表格模型
    //初始化显示的QSqlTableModel：
    queryModel = new QSqlQueryModel(); //Q_NULLPTR
    //设置一个定时器每秒将数据送往数据库
    QTimer *timer = new QTimer;
    timer->start(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(addDataToDb()));
    //初始化timeEdit,dateEdit
    ui->dateEdit->setDate(QDate::currentDate());
    ui->timeEdit_end->setTime(QTime(23,59,59));
    // 设置显示格式
    ui->timeEdit_start->setDisplayFormat("HH:mm:ss");
    ui->timeEdit_end->setDisplayFormat("HH:mm:ss");
    //图像显示初始化
    customplotInit(ui->qcustom);
}

Query::~Query()
{
    delete ui;
}

void Query::on_pushButton_back_clicked()
{
    emit toHomePage();
    this->hide();
}

bool Query::connectDatebase(){
    //this->db.addDatabase("QSQLITE");
    //db.addDatabase("QSQLITE");
    db.setDatabaseName("../monitor/monitor.db");

    if(db.open()){ //打开连接
        qDebug()<<"database connection is established";
        qDebug()<<db.tables()<<"----";
        dbConnectSuccess = true;
        return true;
    }
    else{
        qDebug()<<"database connection is error";
        qDebug()<<db.lastError();
        dbConnectSuccess = false;
        return false;
    }
}

//注意数据库连接成功后才能创建TableModel
bool Query::createTableModel(){
    if(dbConnectSuccess == true){ //数据库连接成功
        tableModel = new QSqlTableModel(Q_NULLPTR,db);
        bool isSuccess; //创建模型是否成功
        tableModel->setEditStrategy(QSqlTableModel::OnManualSubmit);//QSqlTableModel::OnManualSubmit
    //    //设置table为student表
        tableModel->setTable("train"); //设置具体的表
        isSuccess = tableModel->select(); //用数据填充模型
        if(isSuccess == true){
            qDebug()<<"TableModel built successfully";
            dbCreateTableModelSuccess = true; //数据库创建表格模型创建成功
            return true;
        }
        else{
            qDebug()<<"TableModel built failed";
            qDebug()<<tableModel->lastError();
            dbCreateTableModelSuccess = false;
            return false;
        }
    } //if(dbConnectSuccess == true)
    else{
        qDebug()<<"数据库连接不成功，不能创建数据表格模型";
        return false;
    }
}
//添加一行新数据到数据库
void Query::addDataToDb(){
    //qDebug()<<"database timer is running..";
    if(dbConnectSuccess == true){
        if(dbCreateTableModelSuccess == true){
           // QString s0,s1,s2,s3,s4;
            trainEnvironment dbData;
            //QQueue<QString> data_queue_db = Dialog::data_queue_db;
            if(MainWindow::data_queue_db.isEmpty() == false) //队列不为空
            {
                QSqlQuery addSQL(db);
                QString SQLstatement;
                dbData = MainWindow::data_queue_db.dequeue(); //出队trainEnvironment类型数据
                qDebug()<<"存入数据库的数据:"<<dbData.time<<' '<<dbData.temperature<<' '<<dbData.humidity<<' '<<dbData.PM10<<' '<<dbData.CO2;
                //插入数据的SQL语句，格式为INSERT INTO train VALUES ('2017-5-4', '16:00:00', '30','40','50','60')
                SQLstatement = "INSERT INTO train VALUES ('"
                        +dbData.date.toString("yyyy-MM-dd")+"','"+dbData.time.toString()+"','"+QString("%1").arg(dbData.temperature)+"','"+QString("%1").arg(dbData.humidity)+"','"
                        +QString("%1").arg(dbData.PM10)+"','"+QString("%1").arg(dbData.CO2) +"')";
                qDebug()<<"存入数据库命令："<<SQLstatement;
                addSQL.exec(SQLstatement);

            }
        } //dbCreateTableModelSuccess == true
        else{
            qDebug("数据库连接成功但TableModel创建失败，尝试再次创建...");
            createTableModel(); //创建表格模型
        }

    } //dbConnectSuccess == true
    else{
        qDebug("数据库连接失败，尝试再次连接...");
        connectDatebase(); //连接数据库
    }
}
/*
 * PutIn:查询的时间
 * 根据时间段查询数据,将查询结果1、返回给queryModel用于在表格显示数据 2、入队queue_record_data用于绘图
 */
void Query::queryData(QDate date_search,QTime time_start_search,QTime time_end_search){
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
    queryModel->setQuery(query); // 将query数据填充到queryModel表格
}
//显示查询返回的数据
void Query::viewSearchData(){
    ui->tableView_recode->setModel(queryModel);
    queryModel->setHeaderData(0, Qt::Horizontal, tr("date"));
    queryModel->setHeaderData(1, Qt::Horizontal, tr("time"));
    queryModel->setHeaderData(2, Qt::Horizontal, tr("temperature"));
    queryModel->setHeaderData(3, Qt::Horizontal, tr("humidity"));
    queryModel->setHeaderData(4, Qt::Horizontal, tr("PM10"));
    queryModel->setHeaderData(5, Qt::Horizontal, tr("CO2"));
    ui->tableView_recode->resizeColumnsToContents(); //列宽适应表格
    ui->tableView_recode->show();
}
//获取查询的条件，并开始查询
void Query::on_pushButton_search_clicked()
{
    time_start_search = ui->timeEdit_start->time();
    time_end_search = ui->timeEdit_end->time();
    date_search = ui->dateEdit->date();
    queue_record_data.clear(); //查询前清除数据
    queryData(date_search,time_start_search,time_end_search);
    //显示数据前先清除原来数据，不然不会刷新显示
    ui->tableView_recode->setModel(NULL);
    viewSearchData();
}
//初始化图形显示
void Query::customplotInit(QCustomPlot *customPlot){
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
    customPlot->graph(2)->setPen(QPen(Qt::black));
    customPlot->graph(2)->setScatterStyle(QCPScatterStyle::ssDisc);
    customPlot->graph(2)->setName("PM2.5");//颗粒物

    customPlot->addGraph(); //
    customPlot->graph(3)->setPen(QPen(Qt::green));
    customPlot->graph(3)->setScatterStyle(QCPScatterStyle::ssDisc);
    customPlot->graph(3)->setName("CO2");

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

    dataTimer.start(1000); // 1s interval to restart
    this->dataTimer.stop();
    connect(&dataTimer, SIGNAL(timeout()), this, SLOT(recordDataSlot()));
}

void Query::recordDataSlot(){
    //QTime time;
    double temp,hum,PM10,CO2;
    if(queue_record_data.isEmpty() == false){
        trainEnvironment tempEn;
        tempEn = queue_record_data.dequeue();
        temp = tempEn.temperature;
        hum = tempEn.humidity;
        PM10 = tempEn.PM10;
        CO2 = tempEn.CO2;
        //time = QTime(tempEn.time.split(":").at(0).toInt(),tempEn.time.split(":").at(1).toInt(),tempEn.time.split(":").at(2).toInt());
        double key; // time elapsed since start of demo, in seconds
        key = (tempEn.time.hour()*3600+tempEn.time.minute()*60+tempEn.time.second());//*100+QTime::currentTime().msec()
        //qDebug()<<"key:"<<key;
        // add data to lines:
        if(BoxTem == true) ui->qcustom->graph(0)->setVisible(true);
        else ui->qcustom->graph(0)->setVisible(false);
        if(BoxHum == true) ui->qcustom->graph(1)->setVisible(true);
        else ui->qcustom->graph(1)->setVisible(false);
        if(BoxPM == true) ui->qcustom->graph(2)->setVisible(true);
        else ui->qcustom->graph(2)->setVisible(false);
        if(BoxCO2 == true) ui->qcustom->graph(3)->setVisible(true);
        else ui->qcustom->graph(3)->setVisible(false);
        if(BoxLegend == true) ui->qcustom->legend->setVisible(true);
        else ui->qcustom->legend->setVisible(false);

        ui->qcustom->graph(0)->addData(key,temp );
        ui->qcustom->graph(1)->addData(key,hum );
        ui->qcustom->graph(2)->addData(key,PM10 );
        ui->qcustom->graph(3)->addData(key,CO2 );
        // rescale value (vertical) axis to fit the current data:
        ui->qcustom->graph(0)->rescaleValueAxis(true);
        ui->qcustom->graph(1)->rescaleValueAxis(true);
        ui->qcustom->graph(2)->rescaleValueAxis(true);
        ui->qcustom->graph(3)->rescaleValueAxis(true);
        // make key axis range scroll with the data (at a constant range size of 8):
        ui->qcustom->xAxis->setRange(key, 60, Qt::AlignRight);
        ui->qcustom->replot();
    }
}

void Query::on_BoxTem_clicked(){
    if(BoxTem == true) BoxTem= false;
    else BoxTem = true;
}

void Query::on_BoxHum_clicked(){
    if(BoxHum == true) BoxHum= false;
    else BoxHum = true;
}

void Query::on_BoxPM_clicked(){
    if(BoxPM == true) BoxPM= false;
    else BoxPM = true;
}

void Query::on_BoxCO2_clicked(){
    if(BoxCO2 == true) BoxCO2= false;
    else BoxCO2 = true;
}

void Query::on_ButtonPause_clicked()
{
    if(dataTimer.isActive()){
        this->dataTimer.stop();
    }
    else {
        this->dataTimer.start();
        qDebug()<<"dataTimer is running...";
    }
}

void Query::on_checkBox_clicked()
{
    if(BoxLegend == true) BoxLegend= false;
    else BoxLegend = true;
}








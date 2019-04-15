#include "serialport.h"
#include "ui_dialog.h"
#include <qdebug.h>
#include <QtMath>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    //获得一次完整的数据后就立即开始解析
    connect(this, SIGNAL(comlpeteDataIsReady()), this, SLOT(update_processed_data()));
    check_existing_serialport_and_update();

}

Dialog::~Dialog()
{
    delete ui;
}

QString Dialog::getBaud(){
    return (ui->comboBox_baud->currentText());
}
QString Dialog::getPort(){
    return (ui->comboBox_port->currentText());
}
QString Dialog::getState(){
    return (ui->comboBox_state->currentText());
}

void Dialog::on_buttonBox_accepted()
{
    qDebug()<<"串口设置完成";
//    if(1){
//        qDebug()<<"creat a QsetialPort object";

//    }
    my_serialport= new QSerialPort();
    qDebug()<<ui->comboBox_port->currentText();
    my_serialport->setPortName(ui->comboBox_port->currentText());
    my_serialport->open(QIODevice::ReadWrite);
        my_serialport->setBaudRate(QSerialPort::Baud9600);
   // my_serialport->setBaudRate(ui->comboBox_baud->currentText().toInt());
    qDebug()<<ui->comboBox_baud->currentText().toInt();
    my_serialport->setDataBits(QSerialPort::Data8);
    my_serialport->setParity(QSerialPort::NoParity);
    my_serialport->setStopBits(QSerialPort::OneStop);
    my_serialport->setFlowControl(QSerialPort::NoFlowControl);
    connect(my_serialport, SIGNAL(readyRead()), this, SLOT(read_raw_data_from_serialPort()));
}

//从串口读取原始数据，并存放在requestData
void Dialog::read_raw_data_from_serialPort()
{
    if (my_serialport != 0)  //如果my_serialport对象存在
    {
        if(my_serialport->isReadable() == true) //如果数据可读
        {
            requestData = my_serialport->readAll();
            //qDebug()<<"the raw data:"<<requestData;

            if(requestData.isEmpty() == false)
            {
                if(put_raw_data_to_queue(requestData) == 1){
                    emit comlpeteDataIsReady();
                }
            }
            else dataIsRight = false;
            requestData.clear();
        }
        else{
            qDebug("data could not read");
        }
    }
    else{
        qDebug("the object is not existed");
    }
}

//以’;‘作为分割处理原始数据将分割好的数据存入队列中
QString str_temp_data; //存储还未传送完成的临时数据
int Dialog::put_raw_data_to_queue(QByteArray data){
    QString qstring = data;
    str_temp_data +=  qstring;
//    qDebug()<<"the data:"<<qstring;
    if(str_temp_data.count(';') >= 1){
        data_queue.enqueue(str_temp_data.split(';').at(0)); //入队一次完整的数据接收;1,20,30,40,50
        for(int i=1;i<str_temp_data.split(';').length();i++) str_temp_data = str_temp_data.split(';').at(i);
        return 1;
    }
    else return 0;
//    qDebug()<<"the queue:"<<data_queue;

}

//将字符串型数据分离出5个数据
int Dialog::process_data_in_queue(void){
    QString str_data;
    if(data_queue.length() != 0){
        str_data = data_queue.dequeue(); //出队 format:"1,20,30,40,50"
        qDebug()<<"接收到的数据:"<<str_data;
        if(str_data.count(',') == 4){//
            QStringList strList = str_data.split(',',QString::SkipEmptyParts);
            if(strList.length() ==  5){
                //装载数据 用于显示和存入数据库
                displayDbData.date = QDate::currentDate();
                displayDbData.time = QTime::currentTime();
                qDebug()<<"time: "<<displayDbData.time.toString();
                displayDbData.trainNumber = strList.at(0).toFloat();
                displayDbData.temperature = strList.at(1).toFloat();
                displayDbData.humidity = strList.at(2).toFloat();
                displayDbData.PM10 = strList.at(3).toFloat();
                displayDbData.CO2 = strList.at(4).toFloat();



                dataIsReady = true;//数据已准备好
            }  //if(strList.length() ==  5)
            else{
                qDebug("数据格式错误");
            }
        }
        else return -1; //数据错误
    }
    else return -1; //数据未跟新
    return 0;
}

void Dialog::update_processed_data(){
    if(process_data_in_queue() != -1){//if data is correct

        dataIsRight = true;
    }
    else dataIsRight = false;
}

void Dialog::on_pushButton_close_clicked()
{
    if(my_serialport->isOpen() == true){
        my_serialport->close();
    }
    else
        qDebug()<<"the serial port is already closed";
}

void Dialog::on_pushButton_update_clicked()
{
    ui->comboBox_port->clear();
    my_serialport->close();  //关闭串口
    check_existing_serialport_and_update();
}

void Dialog::check_existing_serialport_and_update(){
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) //foreach关键字用于遍历容器，第一个参数使用引用
    {
        qDebug() << "Name        : " << info.portName();
        qDebug() << "Description : " << info.description();
        qDebug() << "Manufacturer: " << info.manufacturer();

        //add the existing serial port to comboBox
        QSerialPort serial;
        serial.setPort(info);
        if (serial.open(QIODevice::ReadWrite))
        {
            ui->comboBox_port->addItem(info.portName());
            serial.close();
        }
    }
}






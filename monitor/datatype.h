#ifndef DATATYPE_H
#define DATATYPE_H


 typedef struct{ //创建一个列车环境数据的新类型
    QString trainNumber;
    QDate date;
    QTime time;
    float temperature;
    float humidity;
    float PM10;
    float CO2;
}trainEnvironment;

#endif // DATATYPE_H

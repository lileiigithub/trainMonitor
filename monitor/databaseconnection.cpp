#include <QSqlDatabase>
#include "databaseconnection.h"

//QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL","localhost_3306"); //数据库连接对象,只能以这种方式创建？？//
QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE"); //数据库连接对象,只能以这种方式创建？？

#include "realdatawindow.h"
#include <QApplication>
#include <boot.h>
#include <analysis.h>
#include <query.h>
#include <setting.h>
#include <login.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Login log;
    MainWindow win;
    Boot boot;
    Analysis ana;
    Query que;
    Setting set;

    log.show();
    QObject::connect(&log,SIGNAL(toHomePage()),&boot,SLOT(show()));
    //goto current data page
    QObject::connect(&boot,SIGNAL(currentDataPage()),&win,SLOT(show()));
    QObject::connect(&win,SIGNAL(toHomePage()),&boot,SLOT(show()));
    //goto query page
    QObject::connect(&boot,SIGNAL(queryPage()),&que,SLOT(show()));
    QObject::connect(&que,SIGNAL(toHomePage()),&boot,SLOT(show()));
    //goto analysis page
    QObject::connect(&boot,SIGNAL(analysisPage()),&ana,SLOT(show()));
    QObject::connect(&ana,SIGNAL(toHomePage()),&boot,SLOT(show()));
    //goto setting page
    QObject::connect(&boot,SIGNAL(settingPage()),&set,SLOT(show()));
    QObject::connect(&set,SIGNAL(toHomePage()),&boot,SLOT(show()));
    return a.exec();
}


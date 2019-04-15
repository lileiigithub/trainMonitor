#-------------------------------------------------
#
# Project created by QtCreator 2017-01-08T16:20:19
#
#-------------------------------------------------

QT       += core gui
QT       += serialport
QT       += sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = monitor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    qcustomplot.cpp \
    boot.cpp \
    query.cpp \
    analysis.cpp \
    setting.cpp \
    login.cpp \
    realdatawindow.cpp \
    serialport.cpp \
    databaseconnection.cpp

HEADERS  += \
    qcustomplot.h \
    boot.h \
    query.h \
    analysis.h \
    setting.h \
    login.h \
    realdatawindow.h \
    serialport.h \
    datatype.h \
    databaseconnection.h \
    nothing.h

FORMS    += mainwindow.ui \
    dialog.ui \
    boot.ui \
    query.ui \
    analysis.ui \
    setting.ui \
    login.ui

RESOURCES += \
    icon/ico.qrc

DISTFILES += \
    icon/back.png \
    icon/back_16x16.png \
    icon/back_black.png \
    icon/back_white.png

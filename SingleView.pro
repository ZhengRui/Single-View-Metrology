#-------------------------------------------------
#
# Project created by QtCreator 2014-03-12T23:00:00
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SingleView
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    svmgraphicsview.cpp \
    patchconfirmdialog.cpp

HEADERS  += mainwindow.h \
    svmgraphicsview.h \
    eigen.h \
    patchconfirmdialog.h

FORMS    += mainwindow.ui \
    patchconfirmdialog.ui

OTHER_FILES += \
    roadmap.txt

#-------------------------------------------------
#
# Project created by QtCreator 2013-04-22T10:21:21
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TraceViewer
TEMPLATE = app

TRANSLATIONS += myapp.ts

SOURCES += main.cpp\
        mainwindow.cpp \
    tracewidget.cpp \
    tablemodel.cpp \
    sortfilterproxymodel.cpp \
    filterdialog.cpp \
    detaindialog.cpp \
    textedit.cpp \
    decryptlog.cpp

HEADERS  += mainwindow.h \
    tracewidget.h \
    tablemodel.h \
    sortfilterproxymodel.h \
    filterdialog.h \
    configurefile.h \
    detaindialog.h \
    textedit.h \
    decryptlog.h

RC_FILE = images/traceico.rc

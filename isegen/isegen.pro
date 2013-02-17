#-------------------------------------------------
#
# Project created by QtCreator 2013-02-14T10:36:29
#
#-------------------------------------------------

QT       += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = isegen
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        isexml.cpp \
    boardselect.cpp

HEADERS  += mainwindow.h \
	isexml.h \
    boardselect.h

FORMS    += \
    mainwindow.ui \
    boardselect.ui

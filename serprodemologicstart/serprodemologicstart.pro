#-------------------------------------------------
#
# Project created by QtCreator 2012-03-02T19:03:42
#
#-------------------------------------------------

TARGET = serprodemologicstart
TEMPLATE = app

VERSION=1.0.1

greaterThan(QT_MAJOR_VERSION, 4) {
    QT       += widgets serialport
} else {
    CONFIG += serialport
}

QMAKE_CXXFLAGS+=-Wno-unused-parameter

SOURCES += main.cpp\
        mainwindow.cpp \
        QtSerPro.cpp \
        SerPro/crc16.cpp \
    	portselectdialog.cpp

HEADERS  += mainwindow.h \
		QtSerPro.h \
                SerPro/crc16.h \
                SerPro/SerPro.h \
                SerPro/SerProCommon.h \
                SerPro/SerProHDLC.h \
                SerPro/preprocessor_table.h \
                SerPro/Packet.h \
    		portselectdialog.h

FORMS    += mainwindow.ui \
    portselectdialog.ui


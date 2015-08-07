#-------------------------------------------------
#
# Project created by QtCreator 2015-08-04T14:26:39
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = WifeConnectorServer
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp\
        server.cpp \
    connection.cpp \
    tcpserver.cpp \
    connectionmanager.cpp

HEADERS  += server.h \
    networkproperiesutils.hpp \
    connection.h \
    tcpserver.h \
    connectionmanager.h

FORMS    += server.ui

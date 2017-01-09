#-------------------------------------------------
#
# Project created by QtCreator 2015-05-07T16:47:42
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = Swiftly
TEMPLATE = lib
CONFIG += staticlib

# SOURCES += Swiftly.cpp

HEADERS += Swiftly.h \
    http_parser.h \
    HttpHeader.h \
    HttpRequest.h \
    HttpResponse.h \
    IncommingConnectionQueue.h \
    PathParser.h \
    PathTree.h \
    PathTreeNode.h \
    TaskHandler.h \
    TcpSocket.h \
    WebApp.h \
    Worker.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

SOURCES += \
    HttpHeader.cpp \
    HttpRequest.cpp \
    HttpResponse.cpp \
    HttpServer.cpp \
    IncommingConnectionQueue.cpp \
    PathParser.cpp \
    PathTree.cpp \
    PathTreeNode.cpp \
    TaskHandler.cpp \
    TcpSocket.cpp \
    WebApp.cpp \
    Worker.cpp \
    http_parser.c

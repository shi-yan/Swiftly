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

INCLUDEPATH += ../http-parser

HEADERS += Swiftly.h \
    HttpHeader.h \
    HttpRequest.h \
    HttpResponse.h \
    PathParser.h \
    PathTree.h \
    PathTreeNode.h \
    TaskHandler.h \
    TcpSocket.h \
    WebApp.h \
    Worker.h \
    StaticFileServer.h \
    IncomingConnectionQueue.h \
    WorkerSocketWatchDog.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

SOURCES += \
    HttpHeader.cpp \
    HttpRequest.cpp \
    HttpResponse.cpp \
    Swiftly.cpp \
    PathParser.cpp \
    PathTree.cpp \
    PathTreeNode.cpp \
    TaskHandler.cpp \
    TcpSocket.cpp \
    WebApp.cpp \
    Worker.cpp \
    ../http-parser/http_parser.c \
    StaticFileServer.cpp \
    IncomingConnectionQueue.cpp \
    WorkerSocketWatchDog.cpp

include(../qt-mustache/qt-mustache.pri)

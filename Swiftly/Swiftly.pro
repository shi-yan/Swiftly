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

SOURCES += Swiftly.cpp

HEADERS += Swiftly.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

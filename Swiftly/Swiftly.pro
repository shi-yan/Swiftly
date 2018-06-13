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
CONFIG += c++1z
# SOURCES += Swiftly.cpp

INCLUDEPATH += ../http-parser \
               /usr/local/include/bsoncxx/v_noabi \
               /usr/local/include/mongocxx/v_noabi \
               /usr/local/include \
               /Users/shiyan/mongodb/mongo-cxx-driver/build/install/include/bsoncxx/v_noabi \
               /Users/shiyan/mongodb/mongo-cxx-driver/build/install/include/mongocxx/v_noabi

HEADERS += Swiftly.h \
    HttpHeader.h \
    HttpRequest.h \
    HttpResponse.h \
    PathTree.h \
    PathTreeNode.h \
    TcpSocket.h \
    WebApp.h \
    Worker.h \
    StaticFileServer.h \
    IncomingConnectionQueue.h \
    WorkerSocketWatchDog.h \
    UserManager.h \
    SessionManager.h \
    MongodbManager.h \
    ReCAPTCHAVerifier.h \
    SettingsManager.h \
    SmtpManager.h \
    NetworkServiceAccessor.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}

SOURCES += \
    HttpHeader.cpp \
    HttpRequest.cpp \
    HttpResponse.cpp \
    Swiftly.cpp \
    PathTree.cpp \
    PathTreeNode.cpp \
    TcpSocket.cpp \
    WebApp.cpp \
    Worker.cpp \
    ../http-parser/http_parser.c \
    StaticFileServer.cpp \
    IncomingConnectionQueue.cpp \
    WorkerSocketWatchDog.cpp \
    UserManager.cpp \
    SessionManager.cpp \
    MongodbManager.cpp \
    ReCAPTCHAVerifier.cpp \
    SettingsManager.cpp \
    SmtpManager.cpp \
    NetworkServiceAccessor.cpp

LIBS += -L/usr/local/lib -lsodium
LIBS += -L/Users/shiyan/mongodb/mongo-cxx-driver/build/install/lib
LIBS += -lmongocxx
LIBS += -lbsoncxx

include(../qt-mustache/qt-mustache.pri)
include(../LoggingManager/LoggingManager/LoggingManager.pri)

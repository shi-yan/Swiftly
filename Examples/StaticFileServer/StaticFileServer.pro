#-------------------------------------------------
#
# Project created by QtCreator 2015-05-07T16:47:42
#
#-------------------------------------------------

QT       += network websockets

QT       -= gui

CONFIG += c++11

TARGET = StaticFileServer
TEMPLATE = app

HEADERS += StaticFileServer.h \
    WebsocketServer.h \
    Session.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

SOURCES += main.cpp \
    StaticFileServer.cpp \
    WebsocketServer.cpp \
    Session.cpp


win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../Swiftly/release/ -lSwiftly
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../Swiftly/debug/ -lSwiftly
else:unix: LIBS += -L$$OUT_PWD/../../Swiftly/ -lSwiftly

INCLUDEPATH += $$PWD/../../Swiftly \
               $$PWD/../../http-parser
DEPENDPATH += $$PWD/../../Swiftly

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Swiftly/release/libSwiftly.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Swiftly/debug/libSwiftly.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Swiftly/release/Swiftly.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Swiftly/debug/Swiftly.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../Swiftly/libSwiftly.a

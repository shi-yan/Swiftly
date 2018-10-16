#-------------------------------------------------
#
# Project created by QtCreator 2015-05-07T16:47:42
#
#-------------------------------------------------

QT       += network

QT       -= gui

TARGET = FormSubmission
TEMPLATE = app
CONFIG += c++1z
INCLUDEPATH += $$PWD/../../http-parser

HEADERS += FormSubmission.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

SOURCES += \
    FormSubmission.cpp \
    main.cpp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../Swiftly/release/ -lSwiftly
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../Swiftly/debug/ -lSwiftly
else:unix: LIBS += -L$$OUT_PWD/../../Swiftly/ -lSwiftly

INCLUDEPATH += $$PWD/../../Swiftly
DEPENDPATH += $$PWD/../../Swiftly

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Swiftly/release/libSwiftly.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Swiftly/debug/libSwiftly.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Swiftly/release/Swiftly.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../../Swiftly/debug/Swiftly.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../../Swiftly/libSwiftly.a


LIBS += -L/usr/local/lib -lsodium
LIBS += -L/Users/shiyan/mongodb/mongo-cxx-driver/build/install/lib
LIBS += -lmongocxx
LIBS += -lbsoncxx

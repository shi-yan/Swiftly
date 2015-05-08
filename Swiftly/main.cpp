#include <QCoreApplication>
#include <QtCore/QThread>
#include "httpserver.h"
#include "utilityservice.h"

int main(int argc, char *argv[])
{

    QCoreApplication a(argc, argv);
    REGISTER_WEBAPP(UtilityService);
    HttpServer::getSingleton().start(QThread::idealThreadCount(), 8080);
    return a.exec();
}

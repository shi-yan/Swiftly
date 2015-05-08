#include <QCoreApplication>
#include <QtCore/QThread>
#include "Swiftly.h"
#include "StaticFileServer.h"

int main(int argc, char *argv[])
{
    qDebug() << "Example StaticFileServer";
    QCoreApplication a(argc, argv);
    REGISTER_WEBAPP(StaticFileServer);
    HttpServer::getSingleton().start(QThread::idealThreadCount(), 8083);
    return a.exec();
}

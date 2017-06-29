#include <QCoreApplication>
#include <QtCore/QThread>
#include "Swiftly.h"
#include "StaticServer.h"

int main(int argc, char *argv[])
{
    qDebug() << "Example StaticFileServer";
    QCoreApplication a(argc, argv);
    REGISTER_WEBAPP(StaticServer);
    HttpServer::getSingleton().start(QThread::idealThreadCount(), 8083);
    return a.exec();
}

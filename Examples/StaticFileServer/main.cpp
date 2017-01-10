#include <QCoreApplication>
#include <QtCore/QThread>
#include "Swiftly.h"
#include "StaticFileServer.h"
#include "WebsocketServer.h"

int main(int argc, char *argv[])
{
    qDebug() << "Example StaticFileServer";
    QCoreApplication a(argc, argv);
    REGISTER_WEBAPP(StaticFileServer);
    HttpServer::getSingleton().start(QThread::idealThreadCount(), 8083);
    WebsocketServer websocketServer;
    //websocketServer.listen(QHostAddress::Any, 5678);
    return a.exec();
}

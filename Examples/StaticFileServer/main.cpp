#include <QCoreApplication>
#include <QtCore/QThread>
#include "Swiftly.h"
#include "StaticServer.h"
#include "UserManager.h"

int main(int argc, char *argv[])
{
    qDebug() << "Example StaticFileServer";
    qDebug() << UserManager::isValidEmail("billconan@gmail.com");
    qDebug() << UserManager::isStrongPassword("WW!23wef");

    return 0;
    QCoreApplication a(argc, argv);
    REGISTER_WEBAPP(StaticServer);
    HttpServer::getSingleton().start(QThread::idealThreadCount(), 8083);
    return a.exec();
}

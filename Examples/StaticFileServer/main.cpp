#include <QCoreApplication>
#include <QtCore/QThread>
#include "Swiftly.h"
#include "StaticServer.h"
#include "UserManager.h"
#include <sodium.h>

int main(int argc, char *argv[])
{
    if(sodium_init()!=0)
    {
        qDebug() << "sodium problem";
    }
    qDebug() << "Example StaticFileServer";
    qDebug() << UserManager::isValidEmail("billconan@gmail.com");
    QString errorMessage = "";
    QString password = "123efwef!WEf.";
    QByteArray hash;
    qDebug() << UserManager::isValidePassword(password.toLatin1(), errorMessage);
    UserManager::hashPassword(password.toLatin1(), hash);
    qDebug() << errorMessage << hash;

    qDebug() << "valid password" << UserManager::verifyPassword(hash, password.toLatin1());


    UserManager um;

    um.signup("billconan@gmail.com", "hahahahaHaHa123!");

    return 0;
    QCoreApplication a(argc, argv);
    REGISTER_WEBAPP(StaticServer);
    HttpServer::getSingleton().start(QThread::idealThreadCount(), 8083);
    return a.exec();
}

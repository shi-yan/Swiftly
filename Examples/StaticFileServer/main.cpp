#include <QCoreApplication>
#include <QtCore/QThread>
#include "Swiftly.h"
#include "StaticServer.h"
#include "UserManager.h"
#include <sodium.h>
#include <mongocxx/instance.hpp>


int main(int argc, char *argv[])
{
    if(sodium_init()!=0)
    {
        qDebug() << "sodium problem";
    }
    mongocxx::instance instance{};

    qDebug() << "Example StaticFileServer";
    qDebug() << UserManager::isValidEmail("billconan@gmail.com");
    QString errorMessage = "";
    QString password = "123efwef!WEf.";
    QByteArray hash;
    //qDebug() << UserManager::isValidePassword(password.toLatin1(), errorMessage);
    //UserManager::hashPassword(password.toLatin1(), hash);
    qDebug() << errorMessage << hash;

    //qDebug() << "valid password" << UserManager::verifyPassword(hash, password.toLatin1());


    UserManager um;
    QMap<QString, QVariant> extraFields;

    //um.signup("billconan@gmail.com", "hahahahaHaHa123!", extraFields);
    //um.login("billconan@gmail.com", QString("hahahahaHaHa123!e").toLatin1(), extraFields);
    QByteArray activationCode;
    UserManager::generateActivationCode("billconan@gmail.com", activationCode);

    qDebug() << activationCode;

    return 0;
    QCoreApplication a(argc, argv);
    REGISTER_WEBAPP(StaticServer);
    HttpServer::getSingleton().start(QThread::idealThreadCount(), 8083);
    return a.exec();
}

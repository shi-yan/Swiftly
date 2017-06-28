#include <QCoreApplication>
#include <QtCore/QThread>
#include "Swiftly.h"
#include "UserManagement.h"
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

    qDebug() << "Example UserManagement";

    QCoreApplication a(argc, argv);
    REGISTER_WEBAPP(UserManagement);
    HttpServer::getSingleton().start(QThread::idealThreadCount(), 8083);
    return a.exec();
}

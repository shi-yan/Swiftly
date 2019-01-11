#include <QCoreApplication>
#include <QtCore/QThread>
#include "Swiftly.h"
#include "HelloWorld.h"

int main(int argc, char *argv[])
{
    qDebug() << "Example HelloWorld";
    QCoreApplication::setOrganizationName("Swiftly");
    QCoreApplication::setOrganizationDomain("Swiftly.com");
    QCoreApplication::setApplicationName("HelloWorld example");
    QCoreApplication a(argc, argv);
    REGISTER_WEBAPP(HelloWorld);
    HttpServer::getSingleton().start(1, 8080);
    return a.exec();
}

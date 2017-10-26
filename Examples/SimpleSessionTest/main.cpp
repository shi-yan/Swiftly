#include <QCoreApplication>
#include "Swiftly.h"
#include "SimpleSession.h"

int main(int argc, char *argv[])
{
    qDebug() << "Example SimpleSession";

    QCoreApplication::setOrganizationName("Swiftly");
    QCoreApplication::setOrganizationDomain("Swiftly.com");
    QCoreApplication::setApplicationName("Simple Session Example");

    QCoreApplication a(argc, argv);

    REGISTER_WEBAPP(SimpleSession);

    HttpServer::getSingleton().start(QThread::idealThreadCount(), 8083);

    return a.exec();
}

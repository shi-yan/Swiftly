#include <QCoreApplication>
#include <QtCore/QThread>
#include "Swiftly.h"
#include "UserManagement.h"
#include "UserManagementUI.h"
#include "ReCAPTCHAVerifier.h"

int main(int argc, char *argv[])
{
    qDebug() << "Example UserManagement";
    QCoreApplication a(argc, argv);

    REGISTER_WEBAPP(UserManagement);
    REGISTER_WEBAPP(UserManagementUI);

    HttpServer::getSingleton().start(QThread::idealThreadCount(), 8083);

    return a.exec();
}

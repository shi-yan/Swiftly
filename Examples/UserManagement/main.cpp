#include <QCoreApplication>
#include <QtCore/QThread>
#include "Swiftly.h"
#include "UserManagement.h"
#include "UserManagementUI.h"
#include "ReCAPTCHAVerifier.h"

int main(int argc, char *argv[])
{

    //todo:
    //config file x
    //logging X
    //session x
    //send email x
    //oauth login
    //  google
    //  facebook
    //  github
    //  bitbucket
    //  gitlab
    //  slack
    //upload file


    qDebug() << "Example UserManagement";

    QCoreApplication::setOrganizationName("Swiftly");
    QCoreApplication::setOrganizationDomain("Swiftly.com");
    QCoreApplication::setApplicationName("User Management Example");

    QCoreApplication a(argc, argv);

    REGISTER_WEBAPP(UserManagement);
    REGISTER_WEBAPP(UserManagementUI);

    //UserManagement um;
    //qDebug() << um.sendActivationEmail("shiy@nvidia.com");

    HttpServer::getSingleton().start(QThread::idealThreadCount(), 8083);

    return a.exec();
}

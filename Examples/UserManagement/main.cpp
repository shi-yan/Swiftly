#include <QCoreApplication>
#include <QtCore/QThread>
#include "Swiftly.h"
#include "UserManagementAPI.h"
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

    REGISTER_WEBAPP(UserManagementAPI);
    REGISTER_WEBAPP(UserManagementUI);

    SessionManager::init();
    UserManager::init();

    HttpServer::getSingleton().start(QThread::idealThreadCount(), 8083);

    return a.exec();
}

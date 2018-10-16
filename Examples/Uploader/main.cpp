#include <QCoreApplication>
#include <QtCore/QThread>
#include "Swiftly.h"
#include "Uploader.h"

int main(int argc, char *argv[])
{
    qDebug() << "Example Uploader";
    QCoreApplication a(argc, argv);
    REGISTER_WEBAPP(Uploader);
    HttpServer::getSingleton().start(1, 8080);
    return a.exec();
}

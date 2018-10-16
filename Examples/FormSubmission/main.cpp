#include <QCoreApplication>
#include <QtCore/QThread>
#include "Swiftly.h"
#include "FormSubmission.h"

int main(int argc, char *argv[])
{
    qDebug() << "Example FormSubmission";
    QCoreApplication a(argc, argv);
    REGISTER_WEBAPP(FormSubmission);
    HttpServer::getSingleton().start(1, 8080);
    return a.exec();
}

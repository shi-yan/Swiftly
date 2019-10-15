#include <QCoreApplication>
#include <QtCore/QThread>
#include "Swiftly.h"
#include "HelloWorld.h"
#include <slugify.hpp>
#include <QCryptographicHash>

int main(int argc, char *argv[])
{
    qDebug() << "Example HelloWorld";

    std::string slugified = slugify("Example HelloWorld`'>%3`~\\//{}.,'  7&< > ");
    QString prettyBase = QString::fromStdString(slugified);

    QByteArray random1;
    generateHashCode(random1);

    QByteArray random2;
    generateHashCode(random2);

    QByteArray random1a = QCryptographicHash::hash(random1, QCryptographicHash::Algorithm::Md5);
    QByteArray random2a = QCryptographicHash::hash(random2, QCryptographicHash::Algorithm::Md5);

    QSet<QString> slugCandidates;

    if (prettyBase.isEmpty())
    {
        prettyBase = "abc";
    }

    slugCandidates.insert(prettyBase);
    slugCandidates.insert(prettyBase % "-" % random1a);
    slugCandidates.insert(prettyBase % "-" % random2a);

    if (slugCandidates.size())
    {
        int maxLength = std::numeric_limits<int>::max();
        QString selected;
        for (auto slug : slugCandidates)
        {
            if (slug.length() < maxLength)
            {
                selected = slug;
                maxLength = slug.length();
            }
        }
        qDebug() << "final slug: " << selected;
    }
    else
    {

    }

    qDebug() << "Test: " << prettyBase ;


    QCoreApplication::setOrganizationName("Swiftly");
    QCoreApplication::setOrganizationDomain("Swiftly.com");
    QCoreApplication::setApplicationName("HelloWorld example");
    QCoreApplication a(argc, argv);
    REGISTER_WEBAPP(HelloWorld);
    HttpServer::getSingleton().start(QThread::idealThreadCount(), 8080);
    return a.exec();
}

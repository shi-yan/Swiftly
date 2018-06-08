#include "NetworkServiceAccessor.h"
#include <QNetworkReply>
#include "LoggingManager.h"
#include <QtConcurrent/QtConcurrent>
#include <QThreadPool>
#include <QFuture>
#include <QEventLoop>
#include <QNetworkAccessManager>

NetworkServiceAccessor::NetworkServiceAccessor()
    :QObject()
{

}

QNetworkReply::NetworkError NetworkServiceAccessor::post(const QNetworkRequest &request, const QByteArray &data, QByteArray &replyData) const
{
    QNetworkReply::NetworkError result = QNetworkReply::NoError;
    QFuture<void> future = QtConcurrent::run(QThreadPool::globalInstance(), [&](){
        QEventLoop loop;
        QNetworkAccessManager networkAccessManager;
        QNetworkReply *reply = networkAccessManager.post(request, data);

        connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        LoggingManager::getSingleton().log() << "NetworkService Post: " << request.url().toString();
        qDebug() << "NetworkService Post: " << request.url().toString();
        LoggingManager::getSingleton().log() << "Payload: " << QString::fromUtf8(data);
        qDebug() << "Payload: " << QString::fromUtf8(data);
        loop.exec();

        result = reply->error();

        if (result != QNetworkReply::NoError)
        {
            qDebug() << "Network Service Accessor Error: " << reply->errorString();
            LoggingManager::getSingleton().log() << "NetworkService: " << reply->errorString();
        }

        replyData = reply->readAll();
        qDebug() << replyData;
        reply->deleteLater();
    });

    future.waitForFinished();
    return result;
}

QNetworkReply::NetworkError NetworkServiceAccessor::get(const QNetworkRequest &request, QByteArray &replyData, QList<QNetworkReply::RawHeaderPair> &headers) const
{
    QNetworkReply::NetworkError result = QNetworkReply::NoError;
    QFuture<void> future = QtConcurrent::run(QThreadPool::globalInstance(), [&](){
        QEventLoop loop;

        QNetworkAccessManager networkAccessManager;

        QNetworkReply *reply = networkAccessManager.get(request);
        connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
        LoggingManager::getSingleton().log() << "NetworkService Get: " << request.url().toString();
        qDebug() << "NetworkService Get:" << request.url().toString();
        loop.exec();

        result = reply->error();
        if (result != QNetworkReply::NoError)
        {
            qDebug() << "Network Service Accessor Error: " << reply->errorString();
            qDebug() << "error code:" << reply->error();
            LoggingManager::getSingleton().log() << "NetworkService: " << reply->errorString();
        }

        headers = reply->rawHeaderPairs();
        replyData = reply->readAll();
        qDebug() << replyData;
        reply->deleteLater();});

    future.waitForFinished();
    return result;
}

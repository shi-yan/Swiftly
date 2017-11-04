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

void NetworkServiceAccessor::post(const QNetworkRequest &request, const QByteArray &data, QByteArray &replyData) const
{
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

    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Network Service Accessor Error: " << reply->errorString();
        LoggingManager::getSingleton().log() << "NetworkService: " << reply->errorString();
        qDebug() << reply->readAll();
    }
    else
    {
        replyData = reply->readAll();
        qDebug() << replyData;
    }
    reply->deleteLater();
    });
    future.waitForFinished();
}

void NetworkServiceAccessor::get(const QNetworkRequest &request, QByteArray &replyData) const
{
    QFuture<void> future = QtConcurrent::run(QThreadPool::globalInstance(), [&](){
    QEventLoop loop;

    QNetworkAccessManager networkAccessManager;

    QNetworkReply *reply = networkAccessManager.get(request);
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    LoggingManager::getSingleton().log() << "NetworkService Get: " << request.url().toString();
    qDebug() << "NetworkService Get:" << request.url().toString();
    loop.exec();

    if (reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "Network Service Accessor Error: " << reply->errorString();
        LoggingManager::getSingleton().log() << "NetworkService: " << reply->errorString();
    }
    else
    {
        replyData = reply->readAll();
        qDebug() << replyData;
    }
    reply->deleteLater();});

    future.waitForFinished();
}

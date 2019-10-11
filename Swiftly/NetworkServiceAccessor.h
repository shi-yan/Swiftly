#ifndef NETWORKSERVICEACCESSOR_H
#define NETWORKSERVICEACCESSOR_H

#include <QObject>
#include <QNetworkRequest>
#include <QNetworkReply>

class NetworkServiceAccessor : QObject
{
    Q_OBJECT
private:

public:
    NetworkServiceAccessor();

    QNetworkReply::NetworkError post(const QNetworkRequest &request, const QByteArray &data, QByteArray &reply, bool async = false) const;
    QNetworkReply::NetworkError get(const QNetworkRequest &request, QByteArray &reply, QList<QNetworkReply::RawHeaderPair> &headers) const;
};

#endif // NETWORKSERVICEACCESSER_H

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

    void post(const QNetworkRequest &request, const QByteArray &data, QByteArray &reply) const;
    void get(const QNetworkRequest &request, QByteArray &reply, QList<QNetworkReply::RawHeaderPair> &headers) const;
};

#endif // NETWORKSERVICEACCESSER_H

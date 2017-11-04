#ifndef NETWORKSERVICEACCESSOR_H
#define NETWORKSERVICEACCESSOR_H

#include <QObject>
#include <QNetworkRequest>

class NetworkServiceAccessor : QObject
{
    Q_OBJECT
private:

public:
    NetworkServiceAccessor();

    void post(const QNetworkRequest &request, const QByteArray &data, QByteArray &reply) const;
    void get(const QNetworkRequest &request, QByteArray &reply) const;
};

#endif // NETWORKSERVICEACCESSER_H

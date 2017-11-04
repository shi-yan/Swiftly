#include "ReCAPTCHAVerifier.h"
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include <QThreadPool>

ReCAPTCHAVerifier::ReCAPTCHAVerifier()
{

}

bool ReCAPTCHAVerifier::verify(const QString &response, const QString &ip)
{
    QUrl verificationUrl = m_url;

    QUrlQuery query;

    query.addQueryItem("secret", m_secret);
    query.addQueryItem("response", response);
    query.addQueryItem("remoteip", ip);

    verificationUrl.setQuery(query.query());
    QNetworkRequest request;
    request.setUrl(verificationUrl);
    QByteArray rawData;

    m_networkServiceAccessor.get(request, rawData);

    QJsonParseError error;
    QJsonDocument data = QJsonDocument::fromJson(rawData, &error);

    if (error.error == QJsonParseError::NoError)
    {
        if(data.object().contains("success"))
        {
            bool result = data.object()["success"].toBool();
            return result;
        }
        else
        {
            return false;
        }
    }
    else
    {
        qDebug() << error.errorString();
        return false;
    }
}

void ReCAPTCHAVerifier::init(const QString &secret, const QString &url)
{
    m_secret = secret;
    m_url = QUrl(url);
}

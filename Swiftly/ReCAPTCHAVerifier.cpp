#include "ReCAPTCHAVerifier.h"
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QEventLoop>
#include <QNetworkReply>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>

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

    QNetworkAccessManager networkAccessManager;
    QEventLoop loop;
    QNetworkReply *reply = networkAccessManager.get(request);
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    QByteArray rawData = reply->readAll();
    qDebug() << rawData;
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

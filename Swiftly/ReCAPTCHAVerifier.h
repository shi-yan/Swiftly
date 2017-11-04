#ifndef RECAPTCHAVERIFIER_H
#define RECAPTCHAVERIFIER_H

#include <QString>
#include <QUrl>
#include <QObject>
#include "NetworkServiceAccessor.h"

class ReCAPTCHAVerifier : public QObject
{
    Q_OBJECT
private:
    ReCAPTCHAVerifier();
    QString m_secret;
    QUrl m_url;
    NetworkServiceAccessor m_networkServiceAccessor;

public:
    static ReCAPTCHAVerifier &getSingleton()
    {
        static ReCAPTCHAVerifier obj;
        return obj;
    }

    bool verify(const QString &response, const QString &ip);
    void init(const QString &secret, const QString &url = "https://www.google.com/recaptcha/api/siteverify");
};

#endif // RECAPTCHAVERIFIER_H

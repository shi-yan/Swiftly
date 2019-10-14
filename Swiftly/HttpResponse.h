#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H

#include <QtCore/QObject>
#include <QtNetwork/QTcpSocket>
#include <QtCore/QTextStream>
#include <QtCore/QDataStream>
#include "HttpHeader.h"

class TcpSocket;

class HttpResponse:public QObject
{
    Q_OBJECT


    HttpHeader m_header;
    int m_statusCode;
    QHash<QString, QVariant> m_cookies;
    QString m_sessionId;
    bool m_hasFinished;


public:
    bool m_shouldKeepAlive = false;
    QString m_headerString;
    QByteArray m_buffer;
    HttpResponse();
    HttpResponse(const HttpResponse &in) = delete ;
    void operator=(const HttpResponse &in) = delete;
    ~HttpResponse();

    HttpResponse& operator<<(const QByteArray &in);

    HttpResponse& operator<<(const char *in);

    HttpResponse& operator<<(const QString &in);
    void write(const char*,const size_t size);

    void setSessionId(const QString &_sessionId) {m_sessionId = _sessionId;}

    QByteArray & getBuffer()
    {
        return m_buffer;
    }

    void addCookie(const QString &key, const QVariant &value);

    HttpHeader & getHeader()
    {
        return m_header;
    }

    void finish(const QString &typeOverride = "text/html");


    void setStatusCode(int _statusCode)
    {
        m_statusCode=_statusCode;
    }

    QString getHeader(const QString &headerField) const
    {
        QWeakPointer<QString> header = m_header.getHeaderInfo(headerField);

        if (!header.isNull())
        {
            return *header.data();
        }
        else
        {
            return QString();
        }
    }

    void setHeader(const QString &_headerField, const QSharedPointer<QString> &headerValue)
    {
        m_header.setCurrentHeaderField(_headerField);
        m_header.addHeaderInfo(headerValue);
    }

    void removeHeader(const QString &_headerField)
    {
        m_header.removeHeaderInfo(_headerField);
    }

    void redirectTo(QSharedPointer<QString> url);

};

#endif // HTTPRESPONSE_H

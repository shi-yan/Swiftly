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

    QByteArray m_buffer;
    TcpSocket *m_socket;
    HttpHeader m_header;
    int m_statusCode;
    QMap<QString, QVariant> m_cookies;
    QString m_sessionId;
    bool m_hasFinished;

public:
    HttpResponse(TcpSocket *_socket=0);
    HttpResponse(const HttpResponse &in);
    void operator=(const HttpResponse &in);
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

    QString getHeader(const QString &_headerField) const
    {
        return m_header.getHeaderInfo(_headerField);
    }

    void setHeader(const QString &_headerField,const QString &_headerValue)
    {
        m_header.setCurrentHeaderField(_headerField);
        m_header.addHeaderInfo(_headerValue);
    }

    void removeHeader(const QString &_headerField)
    {
        m_header.removeHeaderInfo(_headerField);
    }

    void redirectTo(const QString &url);

};

#endif // HTTPRESPONSE_H

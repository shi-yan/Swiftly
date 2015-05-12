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

    QByteArray buffer;
    TcpSocket *socket;
    HttpHeader header;
    int statusCode;
    QMap<QString, QVariant> cookies;
    QString sessionId;
    bool hasFinished;

public:

    enum OutputType
    {
        TEXT,
        BINARY
    };

    QString debugInfo;
    HttpResponse(TcpSocket *_socket=0);
    HttpResponse(const HttpResponse &in);
    void operator=(const HttpResponse &in);
    ~HttpResponse();

    HttpResponse& operator<<(const QByteArray &in);

    HttpResponse& operator<<(const char *in);

    HttpResponse& operator<<(const QString &in);
    void write(const char*,const size_t size);

    void setSessionId(const QString &_sessionId) {sessionId = _sessionId;}

    QByteArray & getBuffer()
    {
        return buffer;
    }

    void addCookie(const QString &key, const QVariant &value);

    HttpHeader & getHeader()
    {
        return header;
    }

    void finish(enum OutputType outputType=TEXT);


    void setStatusCode(int _statusCode)
    {
        statusCode=_statusCode;
    }

    QString & getHeader(const QString &_headerField)
    {
        return header.getHeaderInfo(_headerField);
    }

    void setHeader(const QString &_headerField,const QString &_headerValue)
    {
        header.setCurrentHeaderField(_headerField);
        header.addHeaderInfo(_headerValue);
    }

    void removeHeader(const QString &_headerField)
    {
        header.removeHeaderInfo(_headerField);
    }

};

#endif // HTTPRESPONSE_H

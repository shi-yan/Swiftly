#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include <QObject>
#include "HttpHeader.h"
#include <QHostAddress>

class TcpSocket;

class HttpRequest:public QObject
{
    Q_OBJECT

    HttpHeader header;
    QByteArray rawData;
    QMap<QString,QString> formData;
    bool hasSetFormData;

    unsigned int totalBytes;
    unsigned int bytesHaveRead;

    QString rawHeader;
    TcpSocket *socket;

public:
    HttpRequest(TcpSocket *_socket=0);
    HttpRequest(const HttpRequest &in);
    void operator=(const HttpRequest &in);

    void setFormData(const QMap<QString,QString> &_formData)
    {
        formData=_formData;
        hasSetFormData=true;
    }

    QHostAddress getLocalAddress() const;

    HttpHeader & getHeader()
    {
        return header;
    }

    QString & getFormData(const QString &fieldName)
    {
        return formData[fieldName];
    }

    QMap<QString, QString> & getFormData()
    {
        return formData;
    }

    QByteArray & getRawData()
    {
        return rawData;
    }

    bool hasFormData()
    {
        return hasSetFormData;
    }

    void parseFormData();
    void processCookies();

    ~HttpRequest();

    void appendData(const char*,unsigned int);
    void appendData(const QByteArray &ba);

    void setRawHeader(const QString &_rh);

    QString & getRawHeader()
    {
        return rawHeader;
    }

    unsigned int getTotalBytes()
    {
        return totalBytes;
    }

    unsigned int getBytesHaveRead()
    {
        return bytesHaveRead;
    }

    void setTotalBytes(unsigned _totalBytes)
    {
        totalBytes=_totalBytes;
    }
};

#endif // HTTPREQUEST_H

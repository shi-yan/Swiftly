#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include "HttpHeader.h"

class TcpSocket;

class HttpRequest:public QObject
{
    Q_OBJECT

    HttpHeader m_header;
    QByteArray m_rawData;
    QHash<QString, QSharedPointer<QString>> m_formData;
    bool m_hasSetFormData;

    unsigned int m_totalBytes;
    unsigned int m_bytesHaveRead;

    QString m_rawHeader;
public:
    TcpSocket *m_socket;

public:
    HttpRequest(TcpSocket *_socket=0);
    HttpRequest(const HttpRequest &in);
    void operator=(const HttpRequest &in);

    void setFormData(const QHash<QString, QSharedPointer<QString>> &formData);

    HttpHeader & getHeader()
    {
        return m_header;
    }

    QString getFromIPAddress() const;

    QWeakPointer<QString> getFormData(const QString &fieldName)
    {
        if (m_formData.contains(fieldName))
        {
            return m_formData[fieldName].toWeakRef();
        }
        else
        {
            return QWeakPointer<QString>();
        }
    }

    const QHash<QString, QSharedPointer<QString>> & getFormData() const
    {
        return m_formData;
    }

    QByteArray & getRawData()
    {
        return m_rawData;
    }

    bool hasFormData()
    {
        return m_hasSetFormData;
    }

    void parseFormData();
    void processCookies();

    ~HttpRequest();

    void appendData(const char*,unsigned int);
    void appendData(const QByteArray &ba);

    void setRawHeader(const QString &_rh);

    QString & getRawHeader()
    {
        return m_rawHeader;
    }

    unsigned int getTotalBytes()
    {
        return m_totalBytes;
    }

    unsigned int getBytesHaveRead()
    {
        return m_bytesHaveRead;
    }

    void setTotalBytes(unsigned int totalBytes)
    {
        m_totalBytes=totalBytes;
    }
};

#endif // HTTPREQUEST_H

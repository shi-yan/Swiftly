#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include "HttpHeader.h"
#include <QHash>
#include <QVector>

class TcpSocket;

class HttpRequest:public QObject
{
    Q_OBJECT

public:
    class FormData
    {
    public:
        QString m_meta;
        QByteArray m_data;
        QString m_fieldName;
        QHash<QString, QHash<QString, QString>> m_fields;

        FormData(const QString &meta = "", const QByteArray &data= QByteArray()):
            m_meta(meta),
            m_data(data),
            m_fieldName(),
            m_fields(){}

        bool processMeta();
    };

private:

    HttpHeader m_header;
    QByteArray m_rawData;
    QHash<QString, QSharedPointer<FormData>> m_formData;
    bool m_hasSetFormData;

    unsigned int m_totalBytes;
    unsigned int m_bytesHaveRead;

    QString m_rawHeader;
    bool internalParseFormData(const QByteArray &rawData, const QString &boundary, QHash<QString, QSharedPointer<FormData>> &realContent);

    class PatternTracer
    {
    private:
        class Tracker
        {
        public:
            int m_start;
            int m_pointer;

            Tracker(int start = 0, int pOffset = 0)
                :m_start(start),m_pointer(pOffset){}
        };

        QString m_pattern;
        QVector<Tracker> m_trackerList;
        int m_index;
        int m_pOffset;
    public:
        PatternTracer(const QString &pattern, int index, int pOffset = 0)
            :m_pattern(pattern),
              m_trackerList(),
              m_index(index),
              m_pOffset(pOffset)
        {}

        bool push(char c, int &start, int &end)
        {
            for(int i =0;i<m_trackerList.size();)
            {
                if (c == m_pattern[m_trackerList[i].m_pointer+1])
                {
                    m_trackerList[i].m_pointer++;
                    if (m_trackerList[i].m_pointer == m_pattern.size()-1)
                    {
                        start = m_trackerList[i].m_start;
                        end = m_trackerList[i].m_pointer + 1 + start - m_pOffset;

                        return true;
                    }
                    ++i;
                }
                else
                {
                    m_trackerList[i] = m_trackerList[m_trackerList.size() - 1];
                    m_trackerList.pop_back();
                }
            }


            if (c == m_pattern[m_pOffset])
            {
                Tracker t(m_index, m_pOffset);
                m_trackerList.push_back(t);
            }
            m_index++;
            return false;
        }
    };


public:
    TcpSocket *m_socket;
    HttpRequest(TcpSocket *_socket = nullptr);
    HttpRequest(const HttpRequest &in);
    void operator=(const HttpRequest &in);

    HttpHeader & getHeader()
    {
        return m_header;
    }

    QString getFromIPAddress() const;

    QWeakPointer<FormData> getFormData(const QString &fieldName)
    {
        if (m_formData.contains(fieldName))
        {
            return m_formData[fieldName].toWeakRef();
        }
        else
        {
            return QWeakPointer<FormData>();
        }
    }

    const QHash<QString, QSharedPointer<FormData>> & getFormData() const
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

    bool parseFormData();
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

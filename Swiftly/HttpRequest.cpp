#include <QtCore/QStringList>
#include "TcpSocket.h"
#include "HttpRequest.h"
#include <QHostAddress>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

HttpRequest::HttpRequest(const QHostAddress &peerAddress)
    :QObject(),
      m_header(),
      m_rawData(),
      m_formData(),
      m_hasSetFormData(false),
      m_totalBytes(-1),
      m_bytesHaveRead(0),
      m_rawHeader(),
      m_peerAddress(peerAddress)
{
}

HttpRequest::~HttpRequest()
{

}

void HttpRequest::appendData(const char* buffer,unsigned int size)
{
    m_rawData.append(buffer, static_cast<int>(size));
    m_bytesHaveRead+=size;
}

void HttpRequest::appendData(const QByteArray &ba)
{
    m_rawData.append(ba);
    m_bytesHaveRead += static_cast<unsigned int>(ba.count());
}

void HttpRequest::setRawHeader(const QString &_rh)
{
    m_rawHeader=_rh;
}

void HttpRequest::processCookies()
{
    m_header.processCookie();
}

bool HttpRequest::parseFormData()
{
    QWeakPointer<QString> contentLengthString = m_header.getHeaderInfo("Content-Length");

    if(!contentLengthString.isNull())
    {
        //int contentLength=contentLengthString.toInt();

        QWeakPointer<QString> contentTypeString = m_header.getHeaderInfo("Content-Type");

        if(!contentTypeString.isNull())
        {
            QString contentType = *(contentTypeString.data());

            QRegularExpression boundaryRegExp("^multipart/form-data; boundary=([0-9a-zA-Z'()+-_,./:=? ]{0,69}[0-9a-zA-Z'()+-_,./:=?]{1})$");
            QRegularExpressionMatch match = boundaryRegExp.match(contentType);

            if(match.hasMatch())
            {
                QString boundary = match.captured(1);

                if (internalParseFormData(m_rawData, boundary, m_formData))
                {
                    m_hasSetFormData = true;
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else if (contentType == "application/x-www-form-urlencoded")
            {
                //todo doesn't support urlencoded
                return false;
            }
            return false;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool HttpRequest::FormData::processMeta()
{
    QStringList metas = m_meta.split("\r\n", QString::SkipEmptyParts);

    for(int i =0;i<metas.length();++i)
    {
        int sep = metas[i].indexOf(": ");
        if (sep > 0)
        {
            QString field = metas[i].left(sep);
            QString rest = metas[i].mid(sep+2);

            m_fields[field] = QHash<QString, QString>();

            QStringList rests = rest.split("; ", QString::SkipEmptyParts);

            for(int e = 0;e<rests.length();++e)
            {
                int k = rests[e].indexOf('=');
                if (k != -1)
                {
                    QString f = rests[e].left(k);
                    QString v = rests[e].mid(k+1);

                    if (v[0] == '"' && v[v.length()-1] == '"')
                    {
                        v = v.mid(1, v.length()-2);
                    }
                    m_fields[field][f] = v;
                }
                else
                {
                    m_fields[field][rests[e]] = rests[e];
                }
            }
        }
        else
        {
            return false;
        }
    }
    if (m_fields.contains("Content-Disposition"))
    {
        if (m_fields["Content-Disposition"].contains("name"))
        {
            m_fieldName = m_fields["Content-Disposition"]["name"];
        }
    }
    return true;
}

bool HttpRequest::internalParseFormData(const QByteArray &rawData, const QString &boundary, QHash<QString, QVector<QSharedPointer<FormData>>> &realContent)
{
    QString separator = "\r\n--" % boundary % "\r\n";
    QString ending = "\r\n--" % boundary % "--\r\n";
    QString realData = "\r\n\r\n";

    int begin = 0;
    int prevBoundary = -1;
    int prevRealDataBoundary = -1;

    while(begin < rawData.size())
    {
        PatternTracer sepTracer(separator, begin, begin==0?2:0);
        PatternTracer endTracer(ending, begin);
        PatternTracer realDataTracer(realData, begin);
        prevRealDataBoundary = -1;

        int i = begin;

        while(i < rawData.size())
        {
            char c = rawData.data()[i];

            int start = 0;
            int end = 0;

            if (endTracer.push(c, start, end))
            {
                if (prevBoundary != -1 && end == rawData.size() && prevRealDataBoundary != -1 && prevRealDataBoundary + realData.length() <= start)
                {
                    QByteArray metaData(rawData.data() + prevBoundary, prevRealDataBoundary - prevBoundary);

                    QString meta = QString::fromUtf8(metaData);

                    QByteArray actualData(rawData.data() + prevRealDataBoundary + realData.length(), start - (prevRealDataBoundary+realData.length()));

                    QSharedPointer<FormData> formData(new FormData(meta, actualData));

                    if (formData->processMeta() && !formData->m_fieldName.isEmpty())
                    {
                        if (!realContent.contains(formData->m_fieldName))
                        {
                            realContent[formData->m_fieldName] = QVector<QSharedPointer<FormData>>();
                        }

                        realContent[formData->m_fieldName].push_back(formData);
                        return true;
                    }

                    return false;
                }
                return false;
            }
            else if(sepTracer.push(c, start, end))
            {
                if (prevBoundary != -1 && prevRealDataBoundary != -1 && prevRealDataBoundary+realData.length() <= start)
                {
                    QByteArray metaData(rawData.data() + prevBoundary, prevRealDataBoundary - prevBoundary);

                    QString meta = QString::fromUtf8(metaData);

                    QByteArray actualData(rawData.data() + prevRealDataBoundary + realData.length(), start - (prevRealDataBoundary+realData.length()));

                    QSharedPointer<FormData> formData(new FormData(meta, actualData));

                    if (formData->processMeta() && !formData->m_fieldName.isEmpty())
                    {
                        if (!realContent.contains(formData->m_fieldName))
                        {
                            realContent[formData->m_fieldName] = QVector<QSharedPointer<FormData>>();
                        }

                        realContent[formData->m_fieldName].push_back(formData);
                    }
                    else
                    {
                        return false;
                    }
                }

                begin = prevBoundary = end;
                break;
            }
            else if (realDataTracer.push(c, start, end))
            {
                if (prevRealDataBoundary == -1)
                {
                    prevRealDataBoundary = start;
                }
            }
            ++i;
        }
    }

    return false;
}

QString HttpRequest::getFromIPAddress() const
{
    return m_peerAddress.toString();
}

#include <QtCore/QStringList>
#include "TcpSocket.h"
#include "HttpRequest.h"
#include <QHostAddress>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

HttpRequest::HttpRequest(TcpSocket *socket)
    :QObject(),
      m_header(),
      m_rawData(),
      m_formData(),
      m_hasSetFormData(false),
      m_totalBytes(0),
      m_bytesHaveRead(0),
      m_rawHeader(),
      m_socket(socket)
{
}

HttpRequest::HttpRequest(const HttpRequest &in)
    :QObject(),
      m_header(in.m_header),
      m_rawData(in.m_rawData),
      m_formData(in.m_formData),
      m_hasSetFormData(in.m_hasSetFormData),
      m_totalBytes(in.m_totalBytes),
      m_bytesHaveRead(in.m_bytesHaveRead),
      m_rawHeader(in.m_rawHeader),
      m_socket(in.m_socket)
{
}

void HttpRequest::operator=(const HttpRequest &in)
{
    m_header=in.m_header;
    m_rawData=in.m_rawData;
    m_formData=in.m_formData;
    m_hasSetFormData=in.m_hasSetFormData;
    m_totalBytes=in.m_totalBytes;
    m_bytesHaveRead=in.m_bytesHaveRead;
    m_rawHeader=in.m_rawHeader;
    m_socket=in.m_socket;
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

void HttpRequest::parseFormData()
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
                bool success=false;
                QString boundary = match.captured(1);

                if (parseFormData(m_rawData, boundary, m_formData))
                {
                    m_hasSetFormData = true;
                }
                else
                {

                }
            }
            else if (contentType == "application/x-www-form-urlencoded")
            {
                QHash<QString, QSharedPointer<QString>> _formData;

                QStringList pairs = QString(m_rawData).split("&",QString::SkipEmptyParts);

                for(QStringList::Iterator iter = pairs.begin(); iter != pairs.end(); ++iter)
                {
                    QStringList pair = (*iter).split("=",QString::SkipEmptyParts);

                    if (pair.size() == 2)
                    {
                        _formData.insert(pair[0], QSharedPointer<QString>(new QString(pair[1])));
                        m_hasSetFormData=true;

                    }
                }

                if(m_hasSetFormData)
                {
                    m_formData=(_formData);

                }
            }
        }
    }
}

bool HttpRequest::FormData::processMeta()
{

}

bool HttpRequest::parseFormData(const QByteArray &rawData, const QString &boundary, QHash<QString, QSharedPointer<FormData>> &realContent)
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
                        realContent[formData->m_fieldName] = formData;
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
                        realContent[formData->m_fieldName] = formData;
                        return true;
                    }

                    return false;
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
    if (m_socket)
    {
        return m_socket->peerAddress().toString();
    }
    return QString();
}

void HttpRequest::setFormData(const QHash<QString, QSharedPointer<QString>> &formData)
{
    m_formData=formData;
    m_hasSetFormData=true;
}

#include <QtCore/QStringList>
#include "TcpSocket.h"
#include "HttpRequest.h"
#include <QHostAddress>

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
    m_rawData.append(buffer,size);
    m_bytesHaveRead+=size;
}

void HttpRequest::appendData(const QByteArray &ba)
{
    m_rawData.append(ba);
    m_bytesHaveRead+=ba.count();
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
            int i=0;
            for(i=0;i<(*contentTypeString.data()).count();++i)
            {
                if((*contentTypeString.data()).at(i)==';')
                    break;
            }

            QString contentType=(*contentTypeString.data()).mid(0,i);

            if(contentType=="multipart/form-data")
            {
                    bool success=false;

                    QHash<QString, QSharedPointer<QString>> _formData;

                    if((*contentTypeString.data()).mid(i+2,9)=="boundary=")
                    {
                        QString boundary=(*contentTypeString.data()).mid(i+11,(*contentTypeString.data()).count()-i-11);

                        int linebegin=0;
                        int lineend=0;

                        while(m_rawData.at(lineend)!='\r' && m_rawData.at(lineend+1)!='\n')
                        {
                            ++lineend;
                        }

                        QString boundaryCheck = QByteArray(&m_rawData.data()[linebegin],lineend-linebegin);

                        if(boundaryCheck=="--"+boundary)
                        {
                            lineend+=2;
                            linebegin=lineend;

                            while(lineend < m_rawData.count())
                            {
                                while(lineend < m_rawData.count()-1 && m_rawData.at(lineend)!='\r' && m_rawData.at(lineend+1)!='\n')
                                {
                                    ++lineend;
                                }

                                QString fieldCheck= QByteArray(&m_rawData.data()[linebegin],lineend-linebegin);

                                if(!(fieldCheck.left(38)=="Content-Disposition: form-data; name=\""))
                                {
                                    break;
                                }

                                int namelength=38;

                                while(fieldCheck.at(namelength)!='\"' && namelength<fieldCheck.count())
                                {
                                    ++namelength;
                                }

                                QString fieldName=fieldCheck.mid(38,namelength-38);

                                lineend+=2;
                                linebegin=lineend;

                                if(lineend >= m_rawData.count())
                                    break;

                                while(lineend<m_rawData.count()-1 && m_rawData.at(lineend)!='\r' && m_rawData.at(lineend+1)!='\n')
                                {
                                    ++lineend;
                                }

                                lineend+=2;
                                linebegin=lineend;

                                QByteArray value;

                                while(lineend<m_rawData.count())
                                {
                                    while(lineend<m_rawData.count()-1 && m_rawData.at(lineend)!='\r' && m_rawData.at(lineend+1)!='\n')
                                    {
                                        ++lineend;
                                    }

                                    QByteArray thisline(&m_rawData.data()[linebegin],lineend-linebegin);
                                    QString aValueLine=thisline;

                                    if(aValueLine.left(2+boundary.count())=="--"+boundary)
                                    {

                                        _formData[fieldName] = QSharedPointer<QString>(new QString(value));
                                        if(aValueLine.right(2)=="--")
                                        {
                                            success=true;
                                        }

                                        lineend+=2;
                                        linebegin=lineend;

                                        break;
                                    }

                                    value.append(thisline);
                                    value.append('\r');
                                    value.append('\n');

                                    lineend+=2;
                                    linebegin=lineend;
                                }
                            }
                        }
                    }



                if(success)
                {
                    m_formData=(_formData);
                    m_hasSetFormData=true;
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

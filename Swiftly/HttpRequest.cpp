#include <QtCore/QStringList>
#include "TcpSocket.h"
#include "HttpRequest.h"
#include <QHostAddress>

HttpRequest::HttpRequest(TcpSocket *_socket)
    :QObject(),
      header(),
      rawData(),
      formData(),
      hasSetFormData(false),
      totalBytes(0),
      bytesHaveRead(0),
      rawHeader(),
      socket(_socket)
{
}

HttpRequest::HttpRequest(const HttpRequest &in)
    :QObject(),
      header(in.header),
      rawData(in.rawData),
      formData(in.formData),
      hasSetFormData(in.hasSetFormData),
      totalBytes(in.totalBytes),
      bytesHaveRead(in.bytesHaveRead),
      rawHeader(in.rawHeader),
      socket(in.socket)
{
}

void HttpRequest::operator=(const HttpRequest &in)
{
    header=in.header;
    rawData=in.rawData;
    formData=in.formData;
    hasSetFormData=in.hasSetFormData;
    totalBytes=in.totalBytes;
    bytesHaveRead=in.bytesHaveRead;
    rawHeader=in.rawHeader;
    socket=in.socket;
}


HttpRequest::~HttpRequest()
{

}

void HttpRequest::appendData(const char* buffer,unsigned int size)
{
    rawData.append(buffer,size);
    bytesHaveRead+=size;
}

void HttpRequest::appendData(const QByteArray &ba)
{
    rawData.append(ba);
    bytesHaveRead+=ba.count();
}

void HttpRequest::setRawHeader(const QString &_rh)
{
    rawHeader=_rh;
}

void HttpRequest::processCookies()
{
    header.processCookie();
}

void HttpRequest::parseFormData()
{
    QWeakPointer<QString> contentLengthString=header.getHeaderInfo("Content-Length");

    if(!contentLengthString.isNull())
    {
        //int contentLength=contentLengthString.toInt();

        QWeakPointer<QString> contentTypeString=header.getHeaderInfo("Content-Type");

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

                        while(rawData.at(lineend)!='\r' && rawData.at(lineend+1)!='\n')
                        {
                            ++lineend;
                        }

                        QString boundaryCheck= QByteArray(&rawData.data()[linebegin],lineend-linebegin);

                        if(boundaryCheck=="--"+boundary)
                        {
                            lineend+=2;
                            linebegin=lineend;

                            while(lineend<rawData.count())
                            {
                                while(lineend<rawData.count()-1 && rawData.at(lineend)!='\r' && rawData.at(lineend+1)!='\n')
                                {
                                    ++lineend;
                                }

                                QString fieldCheck= QByteArray(&rawData.data()[linebegin],lineend-linebegin);

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

                                if(lineend>=rawData.count())
                                    break;

                                while(lineend<rawData.count()-1 && rawData.at(lineend)!='\r' && rawData.at(lineend+1)!='\n')
                                {
                                    ++lineend;
                                }

                                lineend+=2;
                                linebegin=lineend;

                                QByteArray value;

                                while(lineend<rawData.count())
                                {
                                    while(lineend<rawData.count()-1 && rawData.at(lineend)!='\r' && rawData.at(lineend+1)!='\n')
                                    {
                                        ++lineend;
                                    }

                                    QByteArray thisline(&rawData.data()[linebegin],lineend-linebegin);
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
                    formData=(_formData);
                    hasSetFormData=true;
                }
            }
            else if (contentType == "application/x-www-form-urlencoded")
            {
                QHash<QString, QSharedPointer<QString>> _formData;

                QStringList pairs = QString(rawData).split("&",QString::SkipEmptyParts);

                for(QStringList::Iterator iter = pairs.begin(); iter != pairs.end(); ++iter)
                {
                    QStringList pair = (*iter).split("=",QString::SkipEmptyParts);

                    if (pair.size() == 2)
                    {
                        _formData.insert(pair[0], QSharedPointer<QString>(new QString(pair[1])));
                        hasSetFormData=true;

                    }
                }

                if(hasSetFormData)
                {
                    formData=(_formData);

                }
            }
        }
    }
}

QString HttpRequest::getFromIPAddress() const
{
    if (socket)
    {
        return socket->peerAddress().toString();
    }
    return QString();
}

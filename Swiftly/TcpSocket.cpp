#include "TcpSocket.h"

TcpSocket::TcpSocket(QObject *parent)
    :QTcpSocket(parent),
      isNew(true),
      request(this),
      response(this)
{
}

TcpSocket::TcpSocket(const TcpSocket &in)
    :QTcpSocket(),
      isNew(in.isNew),
      request(in.request),
      response(in.response)
{
    (*this)=in;
    setSocketDescriptor(in.socketDescriptor());
}

void TcpSocket::operator=(const TcpSocket &in)
{
    (*this)=in;
    isNew=in.isNew;
    request=in.request;
    response=in.response;
}

TcpSocket::~TcpSocket()
{
}

void TcpSocket::appendData(const char* buffer,unsigned int size)
{
    request.appendData(buffer,size);
}

void TcpSocket::appendData(const QByteArray &buffer)
{
    request.appendData(buffer);
}

void TcpSocket::setRawHeader(const QString &in)
{
    request.setRawHeader(in);
}

QString & TcpSocket::getRawHeader()
{
    return request.getRawHeader();
}

unsigned int TcpSocket::getTotalBytes()
{
    return request.getTotalBytes();
}

unsigned int TcpSocket::getBytesHaveRead()
{
    return request.getBytesHaveRead();
}

HttpHeader & TcpSocket::getHeader()
{
    return request.getHeader();
}

bool TcpSocket::isEof()
{
    return (isNew==false) && (request.getTotalBytes()<=request.getBytesHaveRead());
}

void TcpSocket::notNew()
{
    isNew=false;
}

bool TcpSocket::isNewSocket()
{
    return isNew;
}

void TcpSocket::setTotalBytes(unsigned int _totalBytes)
{
    request.setTotalBytes(_totalBytes);
}


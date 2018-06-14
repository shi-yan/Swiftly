#include "TcpSocket.h"

TcpSocket::TcpSocket(QObject *parent)
    :QTcpSocket(parent),
      m_isNew(true),
      m_request(this),
      m_response(this)
{
}

TcpSocket::TcpSocket(const TcpSocket &in)
    :QTcpSocket(),
      m_isNew(in.m_isNew),
      m_request(in.m_request),
      m_response(in.m_response)
{
    (*this)=in;
    setSocketDescriptor(in.socketDescriptor());
}

void TcpSocket::operator=(const TcpSocket &in)
{
    (*this)=in;
    m_isNew=in.m_isNew;
    m_request=in.m_request;
    m_response=in.m_response;
}

TcpSocket::~TcpSocket()
{
}

void TcpSocket::appendData(const char* buffer,unsigned int size)
{
    m_request.appendData(buffer,size);
}

void TcpSocket::appendData(const QByteArray &buffer)
{
    m_request.appendData(buffer);
}

void TcpSocket::setRawHeader(const QString &in)
{
    m_request.setRawHeader(in);
}

QString & TcpSocket::getRawHeader()
{
    return m_request.getRawHeader();
}

unsigned int TcpSocket::getTotalBytes()
{
    return m_request.getTotalBytes();
}

unsigned int TcpSocket::getBytesHaveRead()
{
    return m_request.getBytesHaveRead();
}

HttpHeader & TcpSocket::getHeader()
{
    return m_request.getHeader();
}

bool TcpSocket::isEof()
{
    return (m_isNew == false) && (m_request.getTotalBytes() <= m_request.getBytesHaveRead());
}

void TcpSocket::notNew()
{
    m_isNew = false;
}

bool TcpSocket::isNewSocket()
{
    return m_isNew;
}

void TcpSocket::setTotalBytes(unsigned int totalBytes)
{
    m_request.setTotalBytes(totalBytes);
}


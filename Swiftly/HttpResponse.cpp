#include "HttpResponse.h"
#include "TcpSocket.h"


HttpResponse::HttpResponse(TcpSocket *_socket)
    :QObject(),
      buffer(),
      socket(_socket),
      header(),
      statusCode(200),
      sessionId(),
      cookies(),
      hasFinished(false)
{
}

HttpResponse::HttpResponse(const HttpResponse &in)
    :QObject(),
      buffer(in.buffer),
      socket(in.socket),
      header(in.header),
      statusCode(in.statusCode),
      sessionId(in.sessionId),
      cookies(in.cookies),
      hasFinished(in.hasFinished)
{

}

void HttpResponse::operator=(const HttpResponse &in)
{
    buffer = in.buffer;
    socket = in.socket;
    header = in.header;
    statusCode = in.statusCode;
    sessionId = in.sessionId;
    cookies = in.cookies;
    hasFinished = in.hasFinished;
}

HttpResponse::~HttpResponse()
{
}

void HttpResponse::addCookie(const QString &key, const QVariant &value)
{
    cookies.insert(key, value);
}

HttpResponse& HttpResponse::operator<<(const QByteArray &in)
{
    buffer.append(in);
    return *this;
}

HttpResponse& HttpResponse::operator<<(const char *in)
{
    buffer.append(in);
    return *this;
}

HttpResponse& HttpResponse::operator<<(const QString &in)
{
    buffer.append(in);
    return *this;
}

void HttpResponse::write(const char* in,const size_t size)
{
    buffer.append(in,size);
}

void HttpResponse::finish(enum OutputType outputType)
{
    if (!hasFinished){
    switch(outputType)
    {
    case BINARY:
    {
        socket->write(buffer);
        break;
    }

    case TEXT:
    default:
    {

          buffer.insert(0,header.toString());

          QString cookieString("Set-Cookie: ");

          if (!sessionId.isEmpty())
          {
              cookieString.append("ssid=").append(sessionId);

              for(QMap<QString, QVariant>::Iterator iter = cookies.begin(); iter != cookies.end(); ++iter)
              {
                  cookieString.append("&").append(iter.key()).append("=").append(iter.value().toString());
              }
          }
          else
          {
              if(cookies.count() > 0)
              {
                  QMap<QString, QVariant>::Iterator iter = cookies.begin();

                  cookieString.append(iter.key()).append("=").append(iter.value().toString());

                  for(++iter; iter != cookies.end(); ++iter)
                  {
                    cookieString.append("&").append(iter.key()).append("=").append(iter.value().toString());
                  }

               }
          }

          if (cookieString.length() > 12)
          {
              cookieString.append("\r\n\r\n");
              buffer.prepend(cookieString.toUtf8());
          }
          else
          {
              buffer.prepend("\r\n\r\n");
          }

          if (statusCode == 200)
          {
              buffer.prepend(QString("HTTP/1.1 %1 Ok\r\n"
                                      "Content-Type: text/html; charset=\"utf-8\"\r\n").arg(statusCode).toUtf8());
          }
          else if (statusCode == 404)
          {
              buffer.prepend(QString("HTTP/1.1 404 Not Found\r\nContent-Type: text/html; charset=\"utf-8\"\r\n\r\n").toUtf8());
          }
          socket->write(buffer);
          break;
      }
      }
    hasFinished = true;
    }
  }

#include "HttpResponse.h"
#include "TcpSocket.h"


HttpResponse::HttpResponse(TcpSocket *_socket)
    :QObject(),
      buffer(),
      socket(_socket),
      header(),
      statusCode(200),
      sessionId(),
      cookies()
{
}

HttpResponse::HttpResponse(const HttpResponse &in)
    :QObject(),
      buffer(in.buffer),
      socket(in.socket),
      header(in.header),
      statusCode(in.statusCode),
      sessionId(in.sessionId),
      cookies(in.cookies)
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
   // qDebug() << "buffer size" << buffer.size();
   // qDebug() << "in size" << in.size();
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
    switch(outputType)
    {
    case BINARY:
    {
        qDebug() << "send size" << buffer.size();

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
              buffer.insert(0, cookieString);
          }
          else
          {
              buffer.insert(0, "\r\n\r\n");
          }

          if (statusCode == 200)
          {
              buffer.insert(0,QString("HTTP/1.1 %1 Ok\r\n"
                                      "Content-Type: text/html; charset=\"utf-8\"\r\n").arg(statusCode));
          }
          else if (statusCode == 404)
          {
              buffer.insert(0,QString("HTTP/1.1 404 Not Found\r\nContent-Type: text/html; charset=\"utf-8\"\r\n\r\n"));
          }
          socket->write(buffer);
          break;
      }
      }
  }

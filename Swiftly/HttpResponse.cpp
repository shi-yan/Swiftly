#include "HttpResponse.h"
#include "TcpSocket.h"

HttpResponse::HttpResponse()
    : QObject(), m_header(), m_statusCode(200), m_cookies(), m_sessionId(), m_hasFinished(false), m_shouldKeepAlive(false), m_headerString(), m_buffer()
{
}

HttpResponse::~HttpResponse()
{
    // qDebug() << "released";
}

void HttpResponse::addCookie(const QString &key, const QVariant &value) { m_cookies.insert(key, value); }

HttpResponse &HttpResponse::operator<<(const QByteArray &in)
{
    m_buffer.append(in);
    return *this;
}

HttpResponse &HttpResponse::operator<<(const char *in)
{
    m_buffer.append(in);
    return *this;
}

HttpResponse &HttpResponse::operator<<(const QString &in)
{
    m_buffer.append(in);
    return *this;
}

void HttpResponse::write(const char *in, const size_t size) { m_buffer.append(in, static_cast<int>(size)); }

void HttpResponse::finish(const QString &typeOverride)
{
    if (!m_hasFinished)
    {
        unsigned int bufferSize = static_cast<unsigned int>(m_buffer.size());

        switch (m_statusCode)
        {
        case 200:
            m_headerString = "HTTP/1.1 " % QString::number(m_statusCode) %
                             " Ok\r\n"
                             "Content-Length: " %
                             QString::number(bufferSize) %
                             "\r\n"
                             "Content-Type: " %
                             typeOverride % "\r\n";
            m_shouldKeepAlive = true;
            break;
        case 302:
            m_headerString = "HTTP/1.1 302 Found\r\n"
                             "Connection:keep-alive\r\n";
            break;
        case 301:
            m_headerString = "HTTP/1.1 301 Moved Permanently\r\n"
                             "Connection:keep-alive\r\n";
            break;
        case 304:
            m_headerString = "HTTP/1.1 304 Not Modified\r\n";
            break;
        case 400:
            m_headerString = "HTTP/1.1 400 Bad Request\r\n";
            break;
        case 401:
            m_headerString = "HTTP/1.1 401 Unauthorized\r\n";
            break;
        case 404:
            m_headerString = "HTTP/1.1 404 Not Found\r\n"
                             "Content-Type: text/html; charset=\"utf-8\"\r\n";
            break;
        case 415:
            m_headerString = "HTTP/1.1 415 Unsupported Media Type\r\n";
            break;
        case 422:
            m_headerString = "HTTP/1.1 422 Unprocessable Entity\r\n";
            break;
        case 500:
            m_headerString = "HTTP/1.1 500 Internal Server Error\r\n";
            break;
        case 503:
            m_headerString = "HTTP/1.1 503 Service Unavilable\r\n";
            break;
        default:
            qDebug() << "unimplemented http status code";
        }

        m_headerString = m_headerString % m_header.toString();

        QString cookieString("Set-Cookie: ");

        if (!m_sessionId.isEmpty())
        {
            cookieString.append("ssid=").append(m_sessionId);
            for (QHash<QString, QVariant>::Iterator iter = m_cookies.begin(); iter != m_cookies.end(); ++iter)
            {
                cookieString.append("&").append(iter.key()).append("=").append(iter.value().toString());
            }
        }
        else if (m_cookies.count() > 0)
        {
            QHash<QString, QVariant>::Iterator iter = m_cookies.begin();

            cookieString.append(iter.key()).append("=").append(iter.value().toString());

            for (++iter; iter != m_cookies.end(); ++iter)
            {
                cookieString.append("&").append(iter.key()).append("=").append(iter.value().toString());
            }
        }

        if ((!m_sessionId.isEmpty()) || (m_cookies.count() > 0))
        {
            m_headerString = m_headerString % cookieString % "; Path=/\r\n";
        }

        m_headerString = m_headerString % "\r\n";
        m_hasFinished  = true;
    }
}

void HttpResponse::redirectTo(QSharedPointer<QString> url)
{
    setStatusCode(302);
    setHeader("Location", url);
    finish();
}

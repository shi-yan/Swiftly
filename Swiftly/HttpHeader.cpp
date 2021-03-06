#include "HttpHeader.h"
#include <QStringList>
#include <QStringBuilder>

HttpHeader::HttpHeader()
    :QObject(),
      m_headerInfo(),
      m_fragment(),
      m_queryString(),
      m_path(),
      m_host(),
      m_url(),
      m_currentHeaderField(),
      m_queries(),
      m_cookies(),
      m_hasQueries(false),
      m_hasCookies(false),
      m_httpMethod(HttpMethod::HTTP_NOMETHOD)
{
}

HttpHeader::~HttpHeader()
{
}

HttpHeader::HttpHeader(const HttpHeader &in)
    :QObject(),
      m_headerInfo(in.m_headerInfo),
      m_fragment(in.m_fragment),
      m_queryString(in.m_queryString),
      m_path(in.m_path),
      m_host(in.m_host),
      m_url(in.m_url),
      m_currentHeaderField(in.m_currentHeaderField),
      m_queries(in.m_queries),
      m_cookies(in.m_cookies),
      m_hasQueries(in.m_hasQueries),
      m_hasCookies(in.m_hasCookies),
      m_httpMethod(in.m_httpMethod)
{
}

void HttpHeader::processCookie()
{
    if (m_headerInfo.contains("Cookie"))
    {
        m_hasCookies = true;

        QStringList cookieGroup = m_headerInfo["Cookie"]->split(";",QString::SkipEmptyParts);

        for (QStringList::Iterator giter = cookieGroup.begin(); giter < cookieGroup.end(); ++giter)
        {
            QStringList cookieTokens = (*giter).trimmed().split("&",QString::SkipEmptyParts);

            for(QStringList::Iterator iter = cookieTokens.begin(); iter < cookieTokens.end(); ++iter)
            {
                QStringList pair = (*iter).split("=");

                if (pair.count() != 2)
                {
                    continue;
                }

                QSharedPointer<QString> value(new QString(pair[1]));

                m_cookies.insert(pair[0], value);
            }
        }
    }
}

void HttpHeader::operator=(const HttpHeader &in)
{
    m_headerInfo = in.m_headerInfo;
    m_fragment = in.m_fragment;
    m_queryString = in.m_queryString;
    m_path = in.m_path;
    m_host = in.m_host;
    m_url = in.m_url;
    m_currentHeaderField = in.m_currentHeaderField;
    m_queries = in.m_queries;
    m_cookies = in.m_cookies;
    m_hasQueries = in.m_hasQueries;
    m_hasCookies = in.m_hasCookies;
    m_httpMethod =in.m_httpMethod;
}

QTextStream & operator<<(QTextStream &ts,  HttpHeader &in)
{
    QHashIterator<QString, QSharedPointer<QString>> i(in.getHeaderInfo());
    while (i.hasNext())
    {
        i.next();
        ts << i.key() << ": " << *i.value().data() << "\r\n";
    }
    return ts;
}

QString HttpHeader::toString()
{
    QString result;
    QHashIterator<QString, QSharedPointer<QString>> i(getHeaderInfo());
    while (i.hasNext())
    {
        i.next();
        result = result % i.key() % ": " % (*i.value().data()) % "\r\n";
    }
    return result;
}

void HttpHeader::setQueryString(const QString &queryString)
{
    m_hasQueries = true;
    m_queryString=queryString;

    QStringList queryTokens = queryString.split("&");

    for(QStringList::Iterator i = queryTokens.begin(); i< queryTokens.end(); ++i)
    {
        QStringList pair = (*i).split("=");

        if (pair.count() != 2)
        {
            continue;
        }

        QSharedPointer<QString> value(new QString(pair[1]));

        m_queries.insert(pair[0], value);
    }
}

void HttpHeader::setHttpMethod(HttpMethod httpMethod)
{
    m_httpMethod = httpMethod;
}

HttpHeader::HttpMethod HttpHeader::getHttpMethod() const
{
    return m_httpMethod;
}

bool HttpHeader::hasQueries() const
{
    return m_hasQueries;
}

bool HttpHeader::hasCookies() const
{
    return m_hasCookies;
}

void HttpHeader::setCurrentHeaderField(const QString &currentHeaderField)
{
    m_currentHeaderField = currentHeaderField;
}

const QHash<QString, QSharedPointer<QString>> &HttpHeader::getCookie() const
{
    return m_cookies;
}

const QHash<QString, QSharedPointer<QString>> &HttpHeader::getHeaderInfo() const
{
    return m_headerInfo;
}

const QHash<QString, QSharedPointer<QString>> &HttpHeader::getQueries() const
{
    return m_queries;
}

QWeakPointer<QString> HttpHeader::getHeaderInfo(const QString & headerField) const
{
    if (m_headerInfo.contains(headerField))
    {
        return m_headerInfo[headerField].toWeakRef();
    }
    else
    {
        return QWeakPointer<QString>();
    }
}

void HttpHeader::removeHeaderInfo(const QString &headerField)
{
    m_headerInfo.remove(headerField);
}

void HttpHeader::addHeaderInfo(const QSharedPointer<QString> &headerValue)
{
    m_headerInfo[m_currentHeaderField] = headerValue;
}

void HttpHeader::setFragment(const QString &fragment)
{
    m_fragment = fragment;
}

void HttpHeader::setPath(const QString &path)
{
    m_path = path;
}

const QString & HttpHeader::getPath() const
{
    return m_path;
}

void HttpHeader::setHost(const QString &host)
{
    m_host = host;
}

void HttpHeader::setUrl(const QString &url)
{
    m_url = url;
}

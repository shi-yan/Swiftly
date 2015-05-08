#include "HttpHeader.h"
#include <QtCore/QStringList>

HttpHeader::HttpHeader()
    :QObject(),
      m_hasCookies(false),
      m_hasQueries(false)
{
}

HttpHeader::~HttpHeader()
{
}

HttpHeader::HttpHeader(const HttpHeader &in)
    :QObject(),
      httpMethod(in.httpMethod),
      headerInfo(in.headerInfo),
      fragment(in.fragment),
      queryString(in.queryString),
      path(in.path),
      host(in.host),
      url(in.url)
{
}

void HttpHeader::processCookie()
{
    if (headerInfo.contains("Cookie"))
    {
        m_hasCookies = true;

        QStringList cookieGroup = headerInfo["Cookie"].split(";",QString::SkipEmptyParts);

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

                cookies.insert(pair[0], pair[1]);
            }
        }
    }
}

void HttpHeader::operator=(const HttpHeader &in)
{
    headerInfo=in.headerInfo;
    fragment=in.fragment;
    queryString=in.queryString;
    path=in.path;
    host=in.host;
    url=in.url;
}

QTextStream & operator<<(QTextStream &ts,  HttpHeader &in)
{
    QMapIterator<QString, QString> i(in.getHeaderInfo());
    while (i.hasNext())
    {
        i.next();
        ts << i.key() << ": " << i.value() << "\r\n";
    }
    return ts;
}

QString HttpHeader::toString()
{
    QString result;
    QMapIterator<QString, QString> i(getHeaderInfo());
    while (i.hasNext())
    {
        i.next();
        result.append(i.key()).append(": ").append(i.value()).append("\r\n");
    }
    return result;
}

void HttpHeader::setQueryString(const QString &_queryString)
{
    m_hasQueries = true;
    queryString=_queryString;

    QStringList queryTokens = queryString.split("&");

    for(QStringList::Iterator i = queryTokens.begin(); i< queryTokens.end(); ++i)
    {
        QStringList pair = (*i).split("=");

        if (pair.count() != 2)
        {
            continue;
        }

        queries.insert(pair[0], pair[1]);
    }
}

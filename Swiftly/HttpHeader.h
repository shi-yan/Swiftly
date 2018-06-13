#ifndef HTTPHEADER_H
#define HTTPHEADER_H

#include <QMap>
#include <QString>
#include <QObject>
#include <QStringBuilder>
#include <QTextStream>

/*! \class HttpHeader
 * This class represents the Http Header of a Http request.
 */

class HttpHeader : public QObject
{
    Q_OBJECT

    // Raw Header in the form of key value pairs
    QMap<QString, QString> m_headerInfo;
    QString m_fragment;
    // Url queries
    QString m_queryString;
    // Url path
    QString m_path;
    // Url host
    QString m_host;
    // Url
    QString m_url;
    QString m_currentHeaderField;

    // Url queries in the form of key value pairs
    QMap<QString, QString> m_queries;
    // Cookies in the form of key value pairs
    QMap<QString, QString> m_cookies;

    bool m_hasQueries;
    bool m_hasCookies;

public:
    enum class HttpMethod
    {
        HTTP_DELETE = 0,
        HTTP_GET,
        HTTP_HEAD,
        HTTP_POST,
        HTTP_PUT,

        /* pathological */
        HTTP_CONNECT,
        HTTP_OPTIONS,
        HTTP_TRACE,

        /* webdav */
        HTTP_COPY,
        HTTP_LOCK,
        HTTP_MKCOL,
        HTTP_MOVE,
        HTTP_PROPFIND,
        HTTP_PROPPATCH,
        HTTP_UNLOCK,

        /* subversion */
        HTTP_REPORT,
        HTTP_MKACTIVITY,
        HTTP_CHECKOUT,
        HTTP_MERGE,

        /* upnp */
        HTTP_MSEARCH,
        HTTP_NOTIFY,
        HTTP_SUBSCRIBE,
        HTTP_UNSUBSCRIBE,
        HTTP_NOMETHOD
    };

    HttpHeader();
    HttpHeader(const HttpHeader &in);
    void operator=(const HttpHeader &in);
    ~HttpHeader();

    void setHttpMethod(HttpMethod httpMethod);
    HttpMethod getHttpMethod() const;

    bool hasQueries() const;
    bool hasCookies() const;

    void setCurrentHeaderField(const QString &currentHeaderField);
    void processCookie();

    const QMap<QString, QString> &getCookie() const;

    const QMap<QString,QString> &getHeaderInfo() const;

    const QMap<QString, QString> &getQueries() const;

    QString getHeaderInfo(const QString & headerField) const;

    void removeHeaderInfo(const QString &headerField);

    void addHeaderInfo(const QString &headerValue);

    void setFragment(const QString &fragment);

    void setQueryString(const QString &queryString);

    void setPath(const QString &path);

    const QString & getPath() const;

    void setHost(const QString &host);

    void setUrl(const QString &url);

    QString toString();

private:
    HttpMethod m_httpMethod;
};

QTextStream & operator<<(QTextStream &ts,  HttpHeader &in);

#endif // HTTPHEADER_H

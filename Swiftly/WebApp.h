#ifndef WEBAPP_H
#define WEBAPP_H

#include <QObject>
#include "HttpResponse.h"
#include "HttpRequest.h"
#include "PathTree.h"
#include "HttpRequest.h"

#define AddGetHandler(path, func) \
 addGetHandler( path, [&](HttpRequest &request, HttpResponse &response){ func (request, response);});

#define AddPostHandler(path, func) \
 addPostHandler( path, [&](HttpRequest &request, HttpResponse &response){ func (request, response);});

template <typename E>
constexpr typename std::underlying_type<E>::type to_underlying(E e) {
    return static_cast<typename std::underlying_type<E>::type>(e);
}

class WebApp:public QObject
{
    Q_OBJECT

    const QString m_pathSpace;

    QSharedPointer<PathTree> m_pathTree;

public:
    WebApp(const QString &_pathSpace="",QObject *parent =0);
    WebApp(const WebApp &in):QObject(),m_pathSpace(in.m_pathSpace){}

    bool addGetHandler(const QString &_path, const std::function<void (HttpRequest &, HttpResponse &)> &in);
    bool addPostHandler(const QString &_path, const std::function<void (HttpRequest &, HttpResponse &)> &in);

    virtual ~WebApp(){}

    const QString & getPathSpace()
    {
        return m_pathSpace;
    }

    virtual void init(){}

    virtual void registerPathHandlers()=0;

    void setPathTree(const QSharedPointer<PathTree> pathtree)
    {
        m_pathTree = pathtree;
    }

    virtual void Serve404(HttpRequest &, HttpResponse &){};

protected:
    void logOffSession(HttpRequest & request);


};

#endif // WEBAPP_H

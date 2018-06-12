#ifndef PATHTREENODE_H
#define PATHTREENODE_H

#include <QObject>
#include <QMap>
#include <functional>
#include "HttpRequest.h"
#include "HttpResponse.h"

class PathTreeNode:public QObject
{
    Q_OBJECT

    QString m_pathName;
    QMap<QString,PathTreeNode> m_children;
    std::function<void(HttpRequest &, HttpResponse &)> m_getTaskHandler;
    std::function<void(HttpRequest &, HttpResponse &)> m_postTaskHandler;

public:

    enum HttpVerb
    {
        GET,
        POST
    };

    PathTreeNode();
    PathTreeNode(const PathTreeNode &in);
    PathTreeNode(const QString _pathName);
    void operator=(const PathTreeNode &in);


    void addChild(const QString &childPathName);
    bool hasChild(const QString &childPathName);

    PathTreeNode& getChild(const QString &childePathName)
    {
        return m_children[childePathName];
    }

    bool setGetHandler(const std::function<void (HttpRequest &, HttpResponse &)> &in);

    bool setPostHandler(const std::function<void (HttpRequest &, HttpResponse &)> &in);

    const std::function<void(HttpRequest &, HttpResponse &)> & getHandler()
    {
        return m_getTaskHandler;
    }

    const std::function<void(HttpRequest &, HttpResponse &)> & postHandler()
    {
        return m_postTaskHandler;
    }

    QString & getPathName()
    {
        return m_pathName;
    }


};

#endif // PATHTREENODE_H

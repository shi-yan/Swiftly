#ifndef PATHTREENODE_H
#define PATHTREENODE_H

#include <QObject>
#include <functional>
#include "HttpRequest.h"
#include "HttpResponse.h"

class PathTreeNode:public QObject
{
    Q_OBJECT

    QString m_pathName;
    QHash<QString, QSharedPointer<PathTreeNode>> m_children;
    std::function<void(HttpRequest &, HttpResponse &)> m_getTaskHandler;
    std::function<void(HttpRequest &, HttpResponse &)> m_postTaskHandler;
    void operator=(const PathTreeNode &in);
    PathTreeNode(const PathTreeNode &in);
public:

    enum HttpVerb
    {
        GET,
        POST
    };

    PathTreeNode();

    PathTreeNode(const QString _pathName);



    void addChild(const QString &childPathName);
    bool hasChild(const QString &childPathName) const;

    QWeakPointer<PathTreeNode> getChild(const QString &childePathName)
    {
        return m_children[childePathName];
    }

    bool setGetHandler(const std::function<void (HttpRequest &, HttpResponse &)> &in);

    bool setPostHandler(const std::function<void (HttpRequest &, HttpResponse &)> &in);

    const std::function<void(HttpRequest &, HttpResponse &)> & getHandler() const
    {
        return m_getTaskHandler;
    }

    const std::function<void(HttpRequest &, HttpResponse &)> & postHandler() const
    {
        return m_postTaskHandler;
    }

    QString & getPathName()
    {
        return m_pathName;
    }

    ~PathTreeNode();

};

#endif // PATHTREENODE_H

#ifndef PATHTREENODE_H
#define PATHTREENODE_H

#include <QObject>
#include "TaskHandler.h"
#include <QMap>

class PathTreeNode:public QObject
{
    Q_OBJECT

    QString pathName;
    TaskHandler getTaskHandler;
    TaskHandler postTaskHandler;
    QMap<QString,PathTreeNode> children;

public:

    enum TaskHandlerType
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

    PathTreeNode *getChild(const QString &childePathName)
    {
        return &children[childePathName];
    }

    bool setGetHandler(QObject *object,const QString methodName);

    bool setPostHandler(QObject *object,const QString methodName);

    const TaskHandler * getHandler()
    {
        return &getTaskHandler;
    }

    const TaskHandler * postHandler()
    {
        return &postTaskHandler;
    }

    QString & getPathName()
    {
        return pathName;
    }


};

#endif // PATHTREENODE_H

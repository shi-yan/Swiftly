#ifndef PATHTREE_H
#define PATHTREE_H

#include <QObject>
#include "PathTreeNode.h"
#include "TaskHandler.h"

class PathTree:public QObject
{
    Q_OBJECT

    PathTreeNode root;


public:
    PathTree(QObject *parent=0);

    bool registerAPath(const QString &path,QObject *object,const QString &methodName,enum PathTreeNode::TaskHandlerType);
    const TaskHandler * getTaskHandlerByPath(const QString &path,enum PathTreeNode::TaskHandlerType);
};

#endif // PATHTREE_H

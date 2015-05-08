#include "PathTreeNode.h"

PathTreeNode::PathTreeNode()
    :QObject(),
      pathName(),
      getTaskHandler(),
      postTaskHandler(),
      children()
{
}

PathTreeNode::PathTreeNode(const PathTreeNode &in)
    :QObject(),
      pathName(in.pathName),
      getTaskHandler(in.getTaskHandler),
      postTaskHandler(in.postTaskHandler),
      children()
{
    children=in.children;
}

PathTreeNode::PathTreeNode(const QString _pathName)
    :QObject(),
      pathName(_pathName),
      getTaskHandler(),
      postTaskHandler(),
      children()
{

}

void PathTreeNode::operator=(const PathTreeNode &in)
{
    pathName=in.pathName;
    getTaskHandler=in.getTaskHandler;
    postTaskHandler=in.postTaskHandler;
    children=in.children;
}


void PathTreeNode::addChild(const QString &childPathName)
{
    PathTreeNode newNode(childPathName);
    children[childPathName]=newNode;
}

bool PathTreeNode::hasChild(const QString &childPathName)
{
    return children.contains(childPathName);
}

bool PathTreeNode::setGetHandler(QObject *object,const QString methodName)
{
    if(getTaskHandler.isEmpty())
    {
        return getTaskHandler.setHandler(object,methodName);
    }
    else
        return false;
}

bool PathTreeNode::setPostHandler(QObject *object, const QString methodName)
{
    if(postTaskHandler.isEmpty())
    {
        return postTaskHandler.setHandler(object,methodName);
    }
    else
        return false;
}

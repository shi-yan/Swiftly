#include "PathTreeNode.h"

PathTreeNode::PathTreeNode()
    :QObject(),
      m_pathName(),
      m_children(),
      m_getTaskHandler(),
      m_postTaskHandler()
{
}

PathTreeNode::PathTreeNode(const PathTreeNode &in)
    :QObject()
{
    Q_UNUSED(in)
}

PathTreeNode::PathTreeNode(const QString pathName)
    :QObject(),
      m_pathName(pathName),
      m_children(),
      m_getTaskHandler(),
      m_postTaskHandler()
{
}

void PathTreeNode::operator=(const PathTreeNode &in)
{
    Q_UNUSED(in)
}

void PathTreeNode::addChild(const QString &childPathName)
{
    QSharedPointer<PathTreeNode> newNode(new PathTreeNode(childPathName));
    m_children[childPathName]=newNode;
}

bool PathTreeNode::hasChild(const QString &childPathName) const
{
    return m_children.contains(childPathName);
}

bool PathTreeNode::setGetHandler(const std::function<void (HttpRequest &, HttpResponse &)> &in)
{
    if(m_getTaskHandler)
    {
        return false;
    }
    else
    {
        m_getTaskHandler = in;
        return true;
    }
}

bool PathTreeNode::setPostHandler(const std::function<void (HttpRequest &, HttpResponse &)> &in)
{
    if(m_postTaskHandler)
    {
        return false;
    }
    else
    {
        m_postTaskHandler = in;
        return true;
    }
}

PathTreeNode::~PathTreeNode()
{
    m_children.clear();
}

#include "PathTreeNode.h"

PathTreeNode::PathTreeNode()
    :QObject(),
      m_pathName(),
      m_getTaskHandler(),
      m_postTaskHandler(),
      m_children()
{
}

PathTreeNode::PathTreeNode(const PathTreeNode &in)
    :QObject(),
      m_pathName(in.m_pathName),
      m_getTaskHandler(in.m_getTaskHandler),
      m_postTaskHandler(in.m_postTaskHandler),
      m_children()
{
    m_children=in.m_children;
}

PathTreeNode::PathTreeNode(const QString pathName)
    :QObject(),
      m_pathName(pathName),
      m_getTaskHandler(),
      m_postTaskHandler(),
      m_children()
{

}

void PathTreeNode::operator=(const PathTreeNode &in)
{
    m_pathName=in.m_pathName;
    m_getTaskHandler=in.m_getTaskHandler;
    m_postTaskHandler=in.m_postTaskHandler;
    m_children=in.m_children;
}


void PathTreeNode::addChild(const QString &childPathName)
{
    PathTreeNode newNode(childPathName);
    m_children[childPathName]=newNode;
}

bool PathTreeNode::hasChild(const QString &childPathName)
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

#include "WebApp.h"
#include "PathTree.h"

WebApp::WebApp(const QString &_pathSpace,QObject *parent )
    :QObject(parent),
      m_pathSpace(_pathSpace)
{

}

bool WebApp::addGetHandler(const QString &_path, const std::function<void (HttpRequest &, HttpResponse &)> &in)
{
    QString path=_path;

    if(!m_pathSpace.isEmpty())
        path='/' + m_pathSpace + _path;

    return m_pathTree->registerAPath(path, in ,PathTreeNode::GET);
}


bool WebApp::addPostHandler(const QString &_path, const std::function<void (HttpRequest &, HttpResponse &)> &in)
{
    QString path=_path;

    if(!m_pathSpace.isEmpty())
        path='/' + m_pathSpace + _path;

    return m_pathTree->registerAPath(path,in,PathTreeNode::POST);
}

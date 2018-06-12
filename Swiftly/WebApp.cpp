#include "WebApp.h"
#include "PathTree.h"

WebApp::WebApp(const QString &_pathSpace,QObject *parent )
    :QObject(parent),
      pathSpace(_pathSpace)
{

}

bool WebApp::addGetHandler(const QString &_path, const std::function<void (HttpRequest &, HttpResponse &)> &in)
{
    QString path=_path;

    if(!pathSpace.isEmpty())
        path='/' + pathSpace + _path;

    return pathTree->registerAPath(path, in ,PathTreeNode::GET);
}


bool WebApp::addPostHandler(const QString &_path, const std::function<void (HttpRequest &, HttpResponse &)> &in)
{
    QString path=_path;

    if(!pathSpace.isEmpty())
        path='/' + pathSpace + _path;

    return pathTree->registerAPath(path,in,PathTreeNode::POST);
}

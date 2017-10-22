#include "WebApp.h"
#include "PathTree.h"

WebApp::WebApp(const QString &_pathSpace,QObject *parent )
    :QObject(parent),
      pathSpace(_pathSpace)
{

}

bool WebApp::addGetHandler(const QString &_path,const QString &handlerName)
{
    QString path=_path;

    if(!pathSpace.isEmpty())
        path='/' + pathSpace + _path;

    QString functionName=handlerName;


    return pathTree->registerAPath(path,this,functionName.append("(HttpRequest&,HttpResponse&)"),PathTreeNode::GET);
}


bool WebApp::addPostHandler(const QString &_path,const QString &handlerName)
{
    QString path=_path;

    if(!pathSpace.isEmpty())
        path='/' + pathSpace + _path;

    QString functionName=handlerName;


    return pathTree->registerAPath(path,this,functionName.append("(HttpRequest&,HttpResponse&)"),PathTreeNode::POST);

}

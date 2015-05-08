#ifndef WEBAPP_H
#define WEBAPP_H

#include <QObject>
#include "HttpResponse.h"
#include "HttpRequest.h"
#include "PathTree.h"
#include "HttpRequest.h"

class WebApp:public QObject
{
    Q_OBJECT

    const QString pathSpace;

    PathTree *pathTree;

public:
    WebApp(const QString &_pathSpace="",QObject *parent =0);
    WebApp(const WebApp &in):QObject(),pathSpace(in.pathSpace){}



    bool addGetHandler(const QString &_path,const QString &handlerName);
    bool addPostHandler(const QString &_path,const QString &handlerName);

    virtual ~WebApp(){}


    const QString & getPathSpace()
    {
        return pathSpace;
    }


    virtual void init(){}

    virtual void registerPathHandlers()=0;

    void setPathTree(PathTree *pt)
    {
        pathTree=pt;
    }

    virtual void Serve404(HttpRequest &, HttpResponse &){};

protected:
    void logOffSession(HttpRequest & request);


};

#endif // WEBAPP_H

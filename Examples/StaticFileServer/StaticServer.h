#ifndef STATICSERVER_H
#define STATICSERVER_H

#include "WebApp.h"
#include "StaticFileServer.h"

class StaticServer : public WebApp
{
    Q_OBJECT

    StaticFileServer m_staticFileServer;
public:
    StaticServer();
    void registerPathHandlers() override;
public slots:
    void handleFileGet(HttpRequest &,HttpResponse &);
};

#endif // STATICFILESERVER_H

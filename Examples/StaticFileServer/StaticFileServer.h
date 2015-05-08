#ifndef STATICFILESERVER_H
#define STATICFILESERVER_H

#include "WebApp.h"

class StaticFileServer : public WebApp
{
    Q_OBJECT

public:
    void registerPathHandlers();
public slots:
    void handleFileGet(HttpRequest &,HttpResponse &);
};

#endif // STATICFILESERVER_H

#ifndef HELLOWORLD_H
#define HELLOWORLD_H

#include "WebApp.h"

class HelloWorld : public WebApp
{
    Q_OBJECT

public:
    void registerPathHandlers();

public slots:
    void handleHelloWorldGet(HttpRequest &,HttpResponse &);
};
#endif // HELLOWORLD_H

#ifndef SIMPLESESSION_H
#define SIMPLESESSION_H

#include "WebApp.h"

class SimpleSession : public WebApp
{
    Q_OBJECT
public:
    SimpleSession();
    void registerPathHandlers() override;

public slots:
    void handleUserSigninGet(HttpRequest &,HttpResponse &);
    void handleUserLoggedInGet(HttpRequest &, HttpResponse &);

};

#endif // SIMPLESESSION_H

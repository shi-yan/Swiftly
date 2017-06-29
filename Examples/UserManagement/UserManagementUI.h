#ifndef USERMANAGEMENTUI_H
#define USERMANAGEMENTUI_H

#include "WebApp.h"
#include "StaticFileServer.h"

class UserManagementUI : public WebApp
{
    Q_OBJECT
    StaticFileServer m_staticFileServer;

public:
    UserManagementUI();
    void registerPathHandlers() override;

public slots:
    void handleLoginUIGet(HttpRequest &, HttpResponse &);
    void handleSignupUIGet(HttpRequest &, HttpResponse &);
    void handleResetPasswordUIGet(HttpRequest &, HttpResponse &);
    void handleResendActivationCodeUIGet(HttpRequest &, HttpResponse &);
    void handleFileGet(HttpRequest &,HttpResponse &);

};

#endif // USERMANAGEMENTUI_H

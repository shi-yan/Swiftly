#ifndef USERMANAGEMENT_H
#define USERMANAGEMENT_H

#include "WebApp.h"
#include "UserManager.h"

class UserManagement : public WebApp
{
    Q_OBJECT

public:
    UserManagement();
    UserManagement(const UserManagement&in)
    {
        m_userManager = in.m_userManager;
    }

    void registerPathHandlers() override;

    UserManager m_userManager;
public slots:
    void handleUserSignupPost(HttpRequest &,HttpResponse &);
    void handleUserLoginPost(HttpRequest &, HttpResponse &);
    void handleUserResetPasswordPost(HttpRequest &, HttpResponse &);
    void handleUserActivationGet(HttpRequest &, HttpResponse &);
    void handleSendPasswordResetRequestGet(HttpRequest &, HttpResponse &);
    void handleSendActivationCodeGet(HttpRequest &, HttpResponse &);
    void handleUserUpdateEmailPost(HttpRequest &, HttpResponse &);
};

#endif // USERMANAGEMENT_H

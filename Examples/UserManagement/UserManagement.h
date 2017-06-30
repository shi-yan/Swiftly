#ifndef USERMANAGEMENT_H
#define USERMANAGEMENT_H

#include "WebApp.h"
#include "UserManager.h"

class UserManagement : public WebApp
{
    Q_OBJECT

    UserManager m_userManager;
public:
    UserManagement();
    UserManagement(const UserManagement&in)
        :WebApp(),
          m_userManager(in.m_userManager)
    {
    }

    void registerPathHandlers() override;

public slots:
    void handleUserSignupPost(HttpRequest &,HttpResponse &);
    void handleUserLoginPost(HttpRequest &, HttpResponse &);
    void handleUserResetPasswordPost(HttpRequest &, HttpResponse &);
    void handleUserActivationGet(HttpRequest &, HttpResponse &);
    void handleSendPasswordResetRequestGet(HttpRequest &, HttpResponse &);
    void handleSendActivationCodeGet(HttpRequest &, HttpResponse &);
    void handleUserUpdateEmailPost(HttpRequest &, HttpResponse &);

public:
    bool sendActivationEmail(const QString &to);

private slots:
    void mailSent(QString status);
};

#endif // USERMANAGEMENT_H

#ifndef USERMANAGEMENT_H
#define USERMANAGEMENT_H

#include "WebApp.h"
#include "UserManager.h"
#include "SessionManager.h"

class UserManagement : public WebApp
{
    Q_OBJECT

    UserManager m_userManager;
    SessionManager m_sessionManager;
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
    void handleUserLogoutPost(HttpRequest &, HttpResponse &);

public:
    bool sendActivationEmail(const QString &to, const QString &activationCode);
    bool sendPasswordResetEmail(const QString &to, const QString &resetCode);

private slots:
    void mailSent(QString status);
};

#endif // USERMANAGEMENT_H

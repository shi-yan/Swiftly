#ifndef USERMANAGEMENT_H
#define USERMANAGEMENT_H

#include "WebApp.h"
#include "UserManager.h"
#include "SessionManager.h"
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonDocument>
#include <QVariant>
#include <QNetworkAccessManager>

class UserManagementAPI : public WebApp
{
    Q_OBJECT

    UserManager m_userManager;
    SessionManager m_sessionManager;

public:
    enum class StatusCode
    {
        NoError,
        JsonParsingFailed,
        MissingParameters,
        reCAPTCHAFailed,
        SendActivationEmailFailed,
        SignupFailed,
        CreateSessionFailed,
        LoginFailed,
        LogoutFailed,
        PasswordResetFailed,
        ActivationFailed,
        SendPasswordResetRequestFailed,
        NoPreviousActivationPending,
        UserFindingFailed,
        AlreadyActivated,
        PasswordResetCodeGenerationFailed,
        NotActivated

    };


    UserManagementAPI();
    UserManagementAPI(const UserManagementAPI&in)
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
    void handleUserLogoutPost(HttpRequest &, HttpResponse &);

    void handleGithubRegisterGet(HttpRequest &, HttpResponse &);


public:
    bool sendActivationEmail(const QString &to, const QString &activationCode);
    bool sendPasswordResetEmail(const QString &to, const QString &resetCode);

private slots:
    void mailSent(QString status);

private:
    inline void respond(HttpResponse &response, const StatusCode status, const QString &errorMessage = QString(),
                        int debugErrorCode = 0, const QMap<QString, QString> &additionalFields = QMap<QString, QString>())
    {
        QJsonObject responseObject;

        responseObject["status"] = to_underlying(status);
        responseObject["debug_error_code"] = debugErrorCode;
        responseObject["error_message"] = errorMessage;

        auto iter = additionalFields.begin();

        for(;iter != additionalFields.end(); ++iter)
        {
            responseObject[iter.key()] = iter.value();
        }

        QJsonDocument responseDoc(responseObject);

        response.setStatusCode(200);
        response << responseDoc.toJson();
        response.finish("application/json");
    }

    inline void respondSuccess(HttpResponse &response)
    {
        respond(response, StatusCode::NoError, "Succeeded!");
    }

    inline void respondParameterMissing(HttpResponse &response)
    {
        respond(response, StatusCode::MissingParameters, "Missing parameters!");
    }

    inline void respondJsonParseError(HttpResponse &response, const QJsonParseError &error)
    {
        respond(response, StatusCode::JsonParsingFailed, error.errorString(), static_cast<int>(error.error));
    }
};

#endif // USERMANAGEMENT_H

#include "UserManagementAPI.h"
#include <QJsonDocument>
#include <QJsonObject>
#include "ReCAPTCHAVerifier.h"
#include "SmtpManager.h"
#include "SettingsManager.h"
#include <QStringBuilder>
#include "LoggingManager.h"

UserManagementAPI::UserManagementAPI()
    : WebApp(),
        m_userManager()
{

}

void UserManagementAPI::registerPathHandlers()
{
    addPostHandler("/api/signup", "handleUserSignupPost");
    addPostHandler("/api/login", "handleUserLoginPost");
    addPostHandler("/api/reset_password", "handleUserResetPasswordPost");
    addGetHandler("/api/activate","handleUserActivationGet");
    addGetHandler("/api/request_password_reset", "handleSendPasswordResetRequestGet");
    addGetHandler("/api/request_activation", "handleSendActivationCodeGet");
    addPostHandler("/api/update_email", "handleUserUpdateEmailPost");
    addPostHandler("/api/logout", "handleUserLogoutPost");
}

void UserManagementAPI::handleUserSignupPost(HttpRequest &request, HttpResponse &response)
{
    QByteArray rawData = request.getRawData();
    QJsonParseError error;
    QJsonDocument data = QJsonDocument::fromJson(rawData, &error);

    if (error.error != QJsonParseError::NoError)
    {
        respondJsonParseError(response, error);
        return;
    }

    QJsonObject dataObject = data.object();

    if (dataObject.contains("email") && dataObject.contains("password") && dataObject.contains("g_recaptcha_response"))
    {
        QString email = dataObject["email"].toString();
        QByteArray password = dataObject["password"].toString().toLatin1();
        QString g_recaptcha_response = dataObject["g_recaptcha_response"].toString();

        if (!ReCAPTCHAVerifier::getSingleton().verify(g_recaptcha_response, request.getFromIPAddress()))
        {
            respond(response, StatusCode::reCAPTCHAFailed, "reCAPTCHA check failed!");
            return;
        }

        QMap<QString, QVariant> extraFields;
        QString errorMessage;
        QByteArray activationCode;
        if(m_userManager.signup(email, password, extraFields, errorMessage, activationCode))
        {
            if (sendActivationEmail(email, QString::fromLatin1(activationCode)))
            {
                respondSuccess(response);
            }
            else
            {
                respond(response, StatusCode::SendActivationEmailFailed, "failed to send activation email!");
            }
        }
        else
        {
            respond(response, StatusCode::SignupFailed, "Signup failed: " % errorMessage);
        }
    }
    else
    {
        respondParameterMissing(response);
    }
}

void UserManagementAPI::handleUserLoginPost(HttpRequest &request, HttpResponse &response)
{
    QByteArray rawData = request.getRawData();
    QJsonParseError error;
    QJsonDocument data = QJsonDocument::fromJson(rawData, &error);

    if (error.error != QJsonParseError::NoError)
    {
        respondJsonParseError(response, error);
        return;
    }

    QJsonObject dataObject = data.object();

    if (dataObject.contains("email") && dataObject.contains("password") && dataObject.contains("g_recaptcha_response"))
    {
        QString email = dataObject["email"].toString();
        QByteArray password = dataObject["password"].toString().toLatin1();
        QString g_recaptcha_response = dataObject["g_recaptcha_response"].toString();

        if (!ReCAPTCHAVerifier::getSingleton().verify(g_recaptcha_response, request.getFromIPAddress()))
        {
            respond(response, StatusCode::reCAPTCHAFailed, "reCAPTCHA check failed!");
            return;
        }

        QMap<QString, QVariant> extraFields;
        QString errorMessage;
        QString userId;
        if(m_userManager.login(email, password, userId, extraFields, errorMessage))
        {
            QByteArray sessionId;
            if (m_sessionManager.newSession(userId, email, sessionId))
            {
                QMap<QString, QString> additionalFields;
                additionalFields["session_id"] = sessionId;
                respond(response, StatusCode::NoError, "Logged in!", 0, additionalFields);
            }
            else
            {
                respond(response, StatusCode::CreateSessionFailed, errorMessage);
            }
        }
        else
        {
            respond(response, StatusCode::LoginFailed, errorMessage);
        }
    }
    else
    {
        respondParameterMissing(response);
    }
}

void UserManagementAPI::handleUserLogoutPost(HttpRequest &request, HttpResponse &response)
{
    QByteArray rawData = request.getRawData();
    QJsonParseError error;
    QJsonDocument data = QJsonDocument::fromJson(rawData, &error);

    if (error.error != QJsonParseError::NoError)
    {
        respondJsonParseError(response, error);
        return;
    }

    QJsonObject dataObject = data.object();

    if (dataObject.contains("session_id"))
    {
        QByteArray session_id = dataObject["session_id"].toString().toLatin1();

        if (!m_sessionManager.logoutSession(session_id))
        {
            respond(response, StatusCode::LogoutFailed, "Logout failed.");
        }
        else
        {
            respondSuccess(response);
        }
    }
    else
    {
        respondParameterMissing(response);
    }
}

void UserManagementAPI::handleUserResetPasswordPost(HttpRequest &request, HttpResponse &response)
{
    QByteArray rawData = request.getRawData();
    QJsonParseError error;
    QJsonDocument data = QJsonDocument::fromJson(rawData, &error);

    if (error.error != QJsonParseError::NoError)
    {
        respondJsonParseError(response, error);
        return;
    }

    QJsonObject dataObject = data.object();

    //not checking if user is activated or not here might be ok.
    //because this api call has to be sent from the ui, as it needs recaptcha
    //and the ui checks if user is activated

    if (dataObject.contains("email") && dataObject.contains("password")
            && dataObject.contains("g_recaptcha_response"))
    {

        QString email = dataObject["email"].toString();
        QByteArray password = dataObject["password"].toString().toLatin1();
        QString g_recaptcha_response = dataObject["g_recaptcha_response"].toString();

        if (!ReCAPTCHAVerifier::getSingleton().verify(g_recaptcha_response, request.getFromIPAddress()))
        {
            respond(response, StatusCode::reCAPTCHAFailed, "reCAPTCHA check failed!");
            return;
        }

        if (dataObject.contains("reset_code"))
        {
            QByteArray reset_code = dataObject["reset_code"].toString().toLatin1();

            if (!m_userManager.resetPassword(email, password, reset_code, false))
            {
                respond(response, StatusCode::PasswordResetFailed, "Password reset failed!");
            }
        }
        else if(dataObject.contains("old_password"))
        {
            QByteArray old_password = dataObject["old_password"].toString().toLatin1();

            if (!m_userManager.resetPassword(email, password, old_password, true))
            {
                respond(response, StatusCode::PasswordResetFailed, "Password reset failed!");
            }
        }
        else
        {
            respondParameterMissing(response);
        }

        respondSuccess(response);
    }
    else
    {
        respondParameterMissing(response);
    }
}

void UserManagementAPI::handleUserActivationGet(HttpRequest &request, HttpResponse &response)
{
    QMap<QString, QString> &queries = request.getHeader().getQueries();

    if (queries.contains("activation_code") && queries.contains("email"))
    {
        QByteArray activationCode = queries["activation_code"].toLatin1();
        QString email = queries["email"];

        if(m_userManager.activate(email, activationCode))
        {
            respondSuccess(response);
        }
        else
        {
            respond(response, StatusCode::ActivationFailed, "Activation failed!");
        }
    }
    else
    {
        respondParameterMissing(response);
    }
}

void UserManagementAPI::handleSendPasswordResetRequestGet(HttpRequest &request, HttpResponse &response)
{
    QMap<QString, QString> &queries = request.getHeader().getQueries();

    if (queries.contains("email") && queries.contains("g_recaptcha_response"))
    {
        QString g_recaptcha_response = queries["g_recaptcha_response"];

        if (!ReCAPTCHAVerifier::getSingleton().verify(g_recaptcha_response, request.getFromIPAddress()))
        {
            respond(response, StatusCode::reCAPTCHAFailed, "reCAPTCHA check failed!");
            return;
        }

        QString email = queries["email"];
        QByteArray passwordResetCode;
        QMap<QString, QVariant> fields;

        if (!m_userManager.getUser(email, fields))
        {
            respond(response, StatusCode::UserFindingFailed, "Failed to find user!");
            return;
        }

        if (!fields.contains("status") || fields["status"].toInt() == 0)
        {
            respond(response, StatusCode::NotActivated, "Account is not activated yet!");
            return;
        }

        if (!m_userManager.generatePasswordResetRequest(email, passwordResetCode))
        {
            respond(response, StatusCode::PasswordResetCodeGenerationFailed, "Failed to generate Password Reset Code!");
            return;
        }

        if (sendPasswordResetEmail(email, QString::fromLatin1(passwordResetCode)))
        {
            respondSuccess(response);
        }
        else
        {
            respond(response, StatusCode::SendActivationEmailFailed,  "Failed to send Password Reset Code!");
        }
    }
    else
    {
        respondParameterMissing(response);
    }
}

void UserManagementAPI::handleSendActivationCodeGet(HttpRequest &request, HttpResponse &response)
{
    QMap<QString, QString> &queries = request.getHeader().getQueries();

    if (queries.contains("email") && queries.contains("g_recaptcha_response"))
    {
        QString g_recaptcha_response = queries["g_recaptcha_response"];

        if (!ReCAPTCHAVerifier::getSingleton().verify(g_recaptcha_response, request.getFromIPAddress()))
        {
            respond(response, StatusCode::reCAPTCHAFailed, "reCAPTCHA check failed!");
            return;
        }

        QString email = queries["email"];
        QByteArray activationCode;

        if (!m_userManager.getActivationCode(email, activationCode))
        {
            respond(response, StatusCode::NoPreviousActivationPending, "No Previous Activation Pending!");
            return;
        }

        QMap<QString, QVariant> fields;

        if (!m_userManager.getUser(email, fields))
        {
            respond(response, StatusCode::UserFindingFailed, "Failed to find user!");
            return;
        }

        if (!fields.contains("status") || fields["status"].toInt() != 0)
        {
            respond(response, StatusCode::AlreadyActivated, "Account is already activated!");
            return;
        }

        if (sendActivationEmail(email, QString::fromLatin1(activationCode)))
        {
            respondSuccess(response);
        }
        else
        {
            respond(response, StatusCode::SendActivationEmailFailed,  "Failed to send Activation Code!");
        }
    }
    else
    {
        respondParameterMissing(response);
    }
}

bool UserManagementAPI::sendActivationEmail(const QString &to, const QString &activationCode)
{
    if (SettingsManager::getSingleton().has("SMTP/username")
            || SettingsManager::getSingleton().has("SMTP/password")
            || SettingsManager::getSingleton().has("SMTP/server")
            || SettingsManager::getSingleton().has("SMTP/from"))
    {
        QString username = SettingsManager::getSingleton().get("SMTP/username").toString();
        QString password = SettingsManager::getSingleton().get("SMTP/password").toString();
        QString server = SettingsManager::getSingleton().get("SMTP/server").toString();
        SmtpManager *smtp = new SmtpManager(username, password, server);
        connect(smtp, SIGNAL(status(QString)), this, SLOT(mailSent(QString)));

        QString emailContent = "http://localhost:8083/activate?activation_code=" % activationCode % "&email=" % to;

        smtp->sendMail("shiyan.nebula@gmail.com", to, "Activate Swiftly", emailContent);
        sLog() << "Send activation email to:" << to << "with activation code:" << activationCode;
        return true;
    }

    return false;
}

bool UserManagementAPI::sendPasswordResetEmail(const QString &to, const QString &resetCode)
{
    if (SettingsManager::getSingleton().has("SMTP/username")
            || SettingsManager::getSingleton().has("SMTP/password")
            || SettingsManager::getSingleton().has("SMTP/server")
            || SettingsManager::getSingleton().has("SMTP/from"))
    {
        QString username = SettingsManager::getSingleton().get("SMTP/username").toString();
        QString password = SettingsManager::getSingleton().get("SMTP/password").toString();
        QString server = SettingsManager::getSingleton().get("SMTP/server").toString();
        SmtpManager *smtp = new SmtpManager(username, password, server);
        connect(smtp, SIGNAL(status(QString)), this, SLOT(mailSent(QString)));

        QString emailContent = "http://localhost:8083/resetPassword?reset_code=" % resetCode % "&email=" % to;

        smtp->sendMail("shiyan.nebula@gmail.com", to, "Reset Swiftly", emailContent);
        sLog() << "Send password reset email to:" << to << "with reset code:" << resetCode;
        return true;
    }

    return false;
}

void UserManagementAPI::mailSent(QString status)
{
    qDebug() << status;
    SmtpManager *smtp = (SmtpManager*)QObject::sender();
    smtp->deleteLater();
    sLog() << "Email sent!";
}

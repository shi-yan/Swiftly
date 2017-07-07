#include "UserManagement.h"
#include <QJsonDocument>
#include <QJsonObject>
#include "ReCAPTCHAVerifier.h"
#include "SmtpManager.h"
#include "SettingsManager.h"
#include <QStringBuilder>

UserManagement::UserManagement()
    : WebApp(),
        m_userManager()
{

}

void UserManagement::registerPathHandlers()
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

void UserManagement::handleUserSignupPost(HttpRequest &request, HttpResponse &response)
{
    QByteArray rawData = request.getRawData();
    QJsonParseError error;
    QJsonDocument data = QJsonDocument::fromJson(rawData, &error);

    if (error.error != QJsonParseError::NoError)
    {
        QJsonObject responseObject;

        responseObject["status"] = 1;
        responseObject["error_category"] = "Json Parsing Issue";
        responseObject["error_code"] = static_cast<int>(error.error);
        responseObject["error_message"] = error.errorString();
        QJsonDocument responseDoc(responseObject);

        response.setStatusCode(200);
        response << responseDoc.toJson();
        response.finish("application/json");
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
            QJsonObject responseObject;

            responseObject["status"] = 4;
            responseObject["error_category"] = "reCAPTCHA check failed.";
            responseObject["error_message"] = "reCAPTCHA check failed!";
            QJsonDocument responseDoc(responseObject);

            response.setStatusCode(200);
            response << responseDoc.toJson();
            response.finish("application/json");
            return;
        }

        QMap<QString, QVariant> extraFields;
        QString errorMessage;
        QByteArray activationCode;
        if(m_userManager.signup(email, password, extraFields, errorMessage, activationCode))
        {
            if (sendActivationEmail(email, QString::fromLatin1(activationCode)))
            {
                QJsonObject responseObject;

                responseObject["status"] = 0;
                QJsonDocument responseDoc(responseObject);

                response.setStatusCode(200);
                response << responseDoc.toJson();
                response.finish("application/json");
            }
            else
            {
                QJsonObject responseObject;

                responseObject["status"] = 5;
                responseObject["error_message"] = "failed to send activation email!";
                QJsonDocument responseDoc(responseObject);

                response.setStatusCode(200);
                response << responseDoc.toJson();
                response.finish("application/json");
            }
            return;
        }
        else
        {
            QJsonObject responseObject;

            responseObject["status"] = 2;
            responseObject["error_category"] = "General Error";
            responseObject["error_message"] = errorMessage;
            QJsonDocument responseDoc(responseObject);

            response.setStatusCode(200);
            response << responseDoc.toJson();
            response.finish("application/json");
            return;
        }
    }
    else
    {
        QJsonObject responseObject;

        responseObject["status"] = 3;
        responseObject["error_category"] = "Missing Infomation";
        responseObject["error_message"] = "Missing Infomation!";
        QJsonDocument responseDoc(responseObject);

        response.setStatusCode(200);
        response << responseDoc.toJson();
        response.finish("application/json");
        return;
    }
}

void UserManagement::handleUserLoginPost(HttpRequest &request, HttpResponse &response)
{
    QByteArray rawData = request.getRawData();
    QJsonParseError error;
    QJsonDocument data = QJsonDocument::fromJson(rawData, &error);

    if (error.error != QJsonParseError::NoError)
    {
        QJsonObject responseObject;

        responseObject["status"] = 1;
        responseObject["error_category"] = "Json Parsing Issue";
        responseObject["error_code"] = static_cast<int>(error.error);
        responseObject["error_message"] = error.errorString();
        QJsonDocument responseDoc(responseObject);

        response.setStatusCode(200);
        response << responseDoc.toJson();
        response.finish("application/json");
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
            QJsonObject responseObject;

            responseObject["status"] = 4;
            responseObject["error_category"] = "reCAPTCHA check failed.";
            responseObject["error_message"] = "reCAPTCHA check failed!";
            QJsonDocument responseDoc(responseObject);

            response.setStatusCode(200);
            response << responseDoc.toJson();
            response.finish("application/json");
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

                QJsonObject responseObject;

                responseObject["status"] = 0;
                responseObject["session_id"] = QString::fromLatin1(sessionId);

                QJsonDocument responseDoc(responseObject);
                response.setStatusCode(200);
                response << responseDoc.toJson();
                response.finish("application/json");
            }
            else
            {
                QJsonObject responseObject;

                responseObject["status"] = 3;
                responseObject["error_message"] = "can't create session";

                QJsonDocument responseDoc(responseObject);
                response.setStatusCode(200);

                response << responseDoc.toJson();
                response.finish("application/json");
            }

            return;
        }
        else
        {
            QJsonObject responseObject;

            responseObject["status"] = 2;
            responseObject["error_category"] = "General Error";
            responseObject["error_message"] = errorMessage;
            QJsonDocument responseDoc(responseObject);

            response.setStatusCode(200);
            response << responseDoc.toJson();
            response.finish("application/json");
            return;
        }
    }
    else
    {
        QJsonObject responseObject;

        responseObject["status"] = 5;
        responseObject["error_category"] = "Missing Infomation";
        QJsonDocument responseDoc(responseObject);

        response.setStatusCode(200);
        response << responseDoc.toJson();
        response.finish("application/json");
        return;
    }
}

void UserManagement::handleUserLogoutPost(HttpRequest &request, HttpResponse &response)
{
    QByteArray rawData = request.getRawData();
    QJsonParseError error;
    QJsonDocument data = QJsonDocument::fromJson(rawData, &error);

    if (error.error != QJsonParseError::NoError)
    {
        QJsonObject responseObject;

        responseObject["status"] = 1;
        responseObject["error_category"] = "Json Parsing Issue";
        responseObject["error_code"] = static_cast<int>(error.error);
        responseObject["error_message"] = error.errorString();
        QJsonDocument responseDoc(responseObject);

        response.setStatusCode(200);
        response << responseDoc.toJson();
        response.finish("application/json");
        return;
    }

    QJsonObject dataObject = data.object();

    if (dataObject.contains("session_id"))
    {
        QByteArray session_id = dataObject["session_id"].toString().toLatin1();

        if (!m_sessionManager.logoutSession(session_id))
        {
            QJsonObject responseObject;

            responseObject["status"] = 4;
            responseObject["error_category"] = "logout failed.";
            QJsonDocument responseDoc(responseObject);

            response.setStatusCode(200);
            response << responseDoc.toJson();
            response.finish("application/json");
            return;
        }
        else
        {
            QJsonObject responseObject;

            responseObject["status"] = 0;
            QJsonDocument responseDoc(responseObject);

            response.setStatusCode(200);
            response << responseDoc.toJson();
            response.finish("application/json");
            return;
        }
    }
    else
    {
        QJsonObject responseObject;

        responseObject["status"] = 3;
        responseObject["error_category"] = "Missing Infomation";
        QJsonDocument responseDoc(responseObject);

        response.setStatusCode(200);
        response << responseDoc.toJson();
        response.finish("application/json");
        return;
    }
}

void UserManagement::handleUserResetPasswordPost(HttpRequest &request, HttpResponse &response)
{
    QByteArray rawData = request.getRawData();
    QJsonParseError error;
    QJsonDocument data = QJsonDocument::fromJson(rawData, &error);

    if (error.error != QJsonParseError::NoError)
    {
        QJsonObject responseObject;

        responseObject["status"] = 1;
        responseObject["error_category"] = "Json Parsing Issue";
        responseObject["error_code"] = static_cast<int>(error.error);
        responseObject["error_message"] = error.errorString();
        QJsonDocument responseDoc(responseObject);

        response.setStatusCode(200);
        response << responseDoc.toJson();
        response.finish("application/json");
        return;

    }

    QJsonObject dataObject = data.object();

    if (dataObject.contains("email") && dataObject.contains("password") && dataObject.contains("reset_code"))
    {
        QString email = dataObject["email"].toString();
        QByteArray password = dataObject["password"].toString().toLatin1();
        QByteArray reset_code = dataObject["reset_code"].toString().toLatin1();

        if (!m_userManager.resetPassword(email, password, reset_code, false))
        {
            QJsonObject responseObject;

            responseObject["status"] = 4;
            responseObject["error_category"] = "reset password failed.";
            QJsonDocument responseDoc(responseObject);

            response.setStatusCode(200);
            response << responseDoc.toJson();
            response.finish("application/json");
            return;
        }
        else
        {
            QJsonObject responseObject;

            responseObject["status"] = 0;
            QJsonDocument responseDoc(responseObject);

            response.setStatusCode(200);
            response << responseDoc.toJson();
            response.finish("application/json");
            return;
        }
    }
    else
    {
        QJsonObject responseObject;

        responseObject["status"] = 3;
        responseObject["error_category"] = "Missing Infomation";
        QJsonDocument responseDoc(responseObject);

        response.setStatusCode(200);
        response << responseDoc.toJson();
        response.finish("application/json");
        return;
    }
}

void UserManagement::handleUserActivationGet(HttpRequest &request, HttpResponse &response)
{
    QMap<QString, QString> &queries = request.getHeader().getQueries();

    if (queries.contains("activation_code") && queries.contains("email"))
    {
        QByteArray activationCode = queries["activation_code"].toLatin1();
        QString email = queries["email"];

        if(m_userManager.activate(email, activationCode))
        {
            QJsonObject responseObject;

            responseObject["status"] = 0;
            QJsonDocument responseDoc(responseObject);
            response.setStatusCode(200);
            response << responseDoc.toJson();
            response.finish("application/json");
            return;
        }
        else
        {
            QJsonObject responseObject;

            responseObject["status"] = 2;
            responseObject["error_category"] = "Can't activate!";
            QJsonDocument responseDoc(responseObject);

            response.setStatusCode(200);
            response << responseDoc.toJson();
            response.finish("application/json");
            return;
        }
    }
    else
    {
        QJsonObject responseObject;

        responseObject["status"] = 1;
        responseObject["error_category"] = "Missing Activation Code";
        QJsonDocument responseDoc(responseObject);

        response.setStatusCode(200);
        response << responseDoc.toJson();
        response.finish("application/json");
        return;
    }
}

void UserManagement::handleSendPasswordResetRequestGet(HttpRequest &request, HttpResponse &response)
{
    QMap<QString, QString> &queries = request.getHeader().getQueries();

    if (queries.contains("email"))
    {
        QString email = queries["email"];

        QByteArray resetCode;

        if(m_userManager.sendPasswordResetRequest(email, resetCode))
        {
            QJsonObject responseObject;

            responseObject["status"] = 0;
            QJsonDocument responseDoc(responseObject);

            response.setStatusCode(200);
            response << responseDoc.toJson();
            response.finish("application/json");
            return;
        }
        else
        {
            QJsonObject responseObject;

            responseObject["status"] = 1;
            responseObject["error_category"] = "Unable to send reset code";
            QJsonDocument responseDoc(responseObject);

            response.setStatusCode(200);
            response << responseDoc.toJson();
            response.finish("application/json");
            return;
        }


    }
    else
    {
        QJsonObject responseObject;

        responseObject["status"] = 2;
        responseObject["error_category"] = "Missing email";
        QJsonDocument responseDoc(responseObject);

        response.setStatusCode(200);
        response << responseDoc.toJson();
        response.finish("application/json");
        return;
    }
}

void UserManagement::handleSendActivationCodeGet(HttpRequest &request, HttpResponse &response)
{
    QMap<QString, QString> &queries = request.getHeader().getQueries();

    if (queries.contains("email"))
    {
        QString email = queries["email"];
        QByteArray activationCode;

        m_userManager.generateActivationCode(email, activationCode);

        if (sendActivationEmail(email, QString::fromLatin1(activationCode)))
        {
            QJsonObject responseObject;

            responseObject["status"] = 0;
            QJsonDocument responseDoc(responseObject);

            response.setStatusCode(200);
            response << responseDoc.toJson();
            response.finish("application/json");
            return;
        }
        else
        {
            QJsonObject responseObject;

            responseObject["status"] = 1;
            responseObject["error_category"] = "Failed to send Activation Code";
            QJsonDocument responseDoc(responseObject);

            response.setStatusCode(200);
            response << responseDoc.toJson();
            response.finish("application/json");
            return;
        }
    }
    else
    {
        QJsonObject responseObject;

        responseObject["status"] = 2;
        responseObject["error_category"] = "Missing email address.";
        QJsonDocument responseDoc(responseObject);

        response.setStatusCode(200);
        response << responseDoc.toJson();
        response.finish("application/json");
        return;
    }
}

void UserManagement::handleUserUpdateEmailPost(HttpRequest &, HttpResponse &)
{

}

bool UserManagement::sendActivationEmail(const QString &to, const QString &activationCode)
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

        QString emailContent = "http://localhost:8083/api/activate?activation_code=" % activationCode % "&email=" % to;

        smtp->sendMail("shiyan.nebula@gmail.com", to, "Activate Swiftly", emailContent);
        return true;
    }

    return false;
}

bool UserManagement::sendPasswordResetEmail(const QString &to, const QString &resetCode)
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
        smtp->sendMail("shiyan.nebula@gmail.com", to, "Reset Swiftly", resetCode);
        return true;
    }

    return false;
}

void UserManagement::mailSent(QString status)
{
    qDebug() << status;
    SmtpManager *smtp = (SmtpManager*)QObject::sender();
    smtp->deleteLater();
}

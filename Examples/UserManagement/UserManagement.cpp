#include "UserManagement.h"
#include <QJsonDocument>
#include <QJsonObject>

UserManagement::UserManagement()
    :WebApp()
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

    if (dataObject.contains("email") && dataObject.contains("password"))
    {
        QString email = dataObject["email"].toString();
        QByteArray password = dataObject["password"].toString().toLatin1();
        QMap<QString, QVariant> extraFields;
        QString errorMessage;
        if(m_userManager.signup(email, password, extraFields,errorMessage))
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
        QJsonDocument responseDoc(responseObject);

        response.setStatusCode(200);
        response << responseDoc.toJson();
        response.finish("application/json");
        return;
    }
}

void UserManagement::handleUserLoginPost(HttpRequest &, HttpResponse &)
{

}

void UserManagement::handleUserResetPasswordPost(HttpRequest &, HttpResponse &)
{

}

void UserManagement::handleUserActivationGet(HttpRequest &, HttpResponse &)
{

}

void UserManagement::handleSendPasswordResetRequestGet(HttpRequest &, HttpResponse &)
{

}

void UserManagement::handleSendActivationCodeGet(HttpRequest &, HttpResponse &)
{

}

void UserManagement::handleUserUpdateEmailPost(HttpRequest &, HttpResponse &)
{

}

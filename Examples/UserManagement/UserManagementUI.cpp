#include "UserManagementUI.h"
#include "SettingsManager.h"
#include <QStringBuilder>

UserManagementUI::UserManagementUI()
    :WebApp(),
      m_staticFileServer(QDir(SettingsManager::getSingleton().get("UserManagement/static_dir", "/home/shiy/startmin").toString())),
      m_templatePath(SettingsManager::getSingleton().get("UserManagement/template_path", ".").toString())
{

}

void UserManagementUI::registerPathHandlers()
{
    addGetHandler("/login", "handleLoginUIGet");
    addGetHandler("/signup", "handleSignupUIGet");
    addGetHandler("/reset_password", "handleResetPasswordUIGet");
    addGetHandler("/loggedInPage", "handleLoggedInPageGet");
    //addGetHandler("/","handleUserActivationGet");
    addGetHandler("/", "handleFileGet");

}

void UserManagementUI::handleLoginUIGet(HttpRequest &request, HttpResponse &response)
{
    QByteArray fileContent;
    QString mimeType;
    if (m_staticFileServer.getFileByAbsolutePath(m_templatePath % "/login.html", fileContent, mimeType))
    {
        response << fileContent;
        response.finish(mimeType);
    }
    else
    {
        response.setStatusCode(404);
        response << "can't find the file!\n";
        response.finish();
    }
}

void UserManagementUI::handleSignupUIGet(HttpRequest &request, HttpResponse &response)
{
    QByteArray fileContent;
    QString mimeType;
    if (m_staticFileServer.getFileByAbsolutePath(m_templatePath % "/signup.html", fileContent, mimeType))
    {
        response << fileContent;
        response.finish(mimeType);
    }
    else
    {
        response.setStatusCode(404);
        response << "can't find the file!\n";
        response.finish();
    }
}

void UserManagementUI::handleResetPasswordUIGet(HttpRequest &request, HttpResponse &response)
{
    QByteArray fileContent;
    QString mimeType;
    if (m_staticFileServer.getFileByAbsolutePath(m_templatePath % "/reset_resetCode.html", fileContent, mimeType))
    {
        response << fileContent;
        response.finish(mimeType);
    }
    else
    {
        response.setStatusCode(404);
        response << "can't find the file!\n";
        response.finish();
    }
}

void UserManagementUI::handleResendActivationCodeUIGet(HttpRequest &request, HttpResponse &response)
{
    QByteArray fileContent;
    QString mimeType;
    if (m_staticFileServer.getFileByAbsolutePath(m_templatePath % "/resend_activation.html", fileContent, mimeType))
    {
        response << fileContent;
        response.finish(mimeType);
    }
    else
    {
        response.setStatusCode(404);
        response << "can't find the file!\n";
        response.finish();
    }
}

void UserManagementUI::handleFileGet(HttpRequest &request,HttpResponse &response)
{
    QByteArray fileContent;
    QString mimeType;
    if (m_staticFileServer.getFileByPath(request.getHeader().getPath(), fileContent, mimeType))
    {
        response << fileContent;
        response.finish(mimeType);
    }
    else
    {
        response.setStatusCode(404);
        response << "can't find the file!\n";
        response.finish();
    }

}

void UserManagementUI::handleLoggedInPageGet(HttpRequest &request, HttpResponse &response)
{
    QMap<QString, QString> &cookies = request.getHeader().getCookie();

    if (cookies.contains("ssid"))
    {
        QByteArray sessionId = cookies["ssid"].toLatin1();
        QString email;
        m_sessionManager.findSession(sessionId, email);

        response.setStatusCode(200);
        response << "find user!\n" << email;
        response << "for session" << sessionId;
        response.finish();
    }
    else
    {
        response.setStatusCode(404);
        response << "can't find session, not logged in?\n";
        response.finish();
    }

}

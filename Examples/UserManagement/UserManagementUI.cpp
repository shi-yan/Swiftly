#include "UserManagementUI.h"

UserManagementUI::UserManagementUI()
    :WebApp(),
      m_staticFileServer(QDir("/home/shiy/startmin"))
{

}

void UserManagementUI::registerPathHandlers()
{
    addGetHandler("/login", "handleLoginUIGet");
    addGetHandler("/signup", "handleSignupUIGet");
    addGetHandler("/reset_password", "handleResetPasswordUIGet");
    //addGetHandler("/","handleUserActivationGet");
    addGetHandler("/", "handleFileGet");
}

void UserManagementUI::handleLoginUIGet(HttpRequest &, HttpResponse &)
{

}

void UserManagementUI::handleSignupUIGet(HttpRequest &request, HttpResponse &response)
{
    QByteArray fileContent;
    QString mimeType;
    if (m_staticFileServer.getFileByPath("/pages/signup.html", fileContent, mimeType))
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

void UserManagementUI::handleResetPasswordUIGet(HttpRequest &, HttpResponse &)
{

}

void UserManagementUI::handleResendActivationCodeUIGet(HttpRequest &, HttpResponse &)
{

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

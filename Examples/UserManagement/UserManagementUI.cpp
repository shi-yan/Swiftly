#include "UserManagementUI.h"
#include "SettingsManager.h"
#include <QStringBuilder>
#include "mustache.h"

UserManagementUI::UserManagementUI()
    :WebApp(),
      m_staticFileServer(QDir(SettingsManager::getSingleton().get("UserManagement/static_dir", "/home/shiy/startmin").toString())),
      m_templatePath(SettingsManager::getSingleton().get("UserManagement/template_path", ".").toString())
{

}

void UserManagementUI::registerPathHandlers()
{
    AddGetHandler("/signup", handleSignupUIGet);
    AddGetHandler("/login", handleLoginUIGet);
    AddGetHandler("/activate", handleUserActivationUIGet);
    AddGetHandler("/resendActivationCode", handleResendActivationCodeUIGet);
    AddGetHandler("/requestPasswordResetCode", handleRequestPasswordResetCodeUIGet);
    AddGetHandler("/resetPassword", handleResetPasswordUIGet);
    AddGetHandler("/loggedInPage", handleLoggedInPageGet);
    AddGetHandler("/", handleFileGet);
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
    QByteArray pageTemplate;
    QString mimeType;
    if (m_staticFileServer.getFileByAbsolutePath(m_templatePath % "/signup.html", pageTemplate, mimeType))
    {        
        QVariantHash info;

        QString client_id = SettingsManager::getSingleton().get("GitHub/client_id").toString();
        QString scope = QString("user:email repo gist").toHtmlEscaped();

        info["gitHub_oauth_link"] = QString("http://github.com/login/oauth/authorize?client_id=" % client_id % "&scpoe=" % scope);

        Mustache::Renderer renderer;
        Mustache::QtVariantContext context(info);

        QString content = renderer.render(QString::fromUtf8(pageTemplate), &context);

        response << content;
        response.finish(mimeType);
    }
    else
    {
        response.setStatusCode(404);
        response << "can't find the file!\n";
        response.finish();
    }
}

void UserManagementUI::handleRequestPasswordResetCodeUIGet(HttpRequest &request, HttpResponse &response)
{
    QByteArray fileContent;
    QString mimeType;
    if (m_staticFileServer.getFileByAbsolutePath(m_templatePath % "/send_resetCode.html", fileContent, mimeType))
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
    QByteArray pageTemplate;
    QString mimeType;
    const QHash<QString, QSharedPointer<QString>> &queries = request.getHeader().getQueries();

    if (m_staticFileServer.getFileByAbsolutePath(m_templatePath % "/resend_activation.html", pageTemplate, mimeType))
    {
        QVariantHash info;

        if (queries.contains("email"))
        {
            info["activation_email"] = *queries["email"].data();
        }
        else
        {
            info["activation_email"] = "";
        }

        Mustache::Renderer renderer;
        Mustache::QtVariantContext context(info);

        QString content = renderer.render(QString::fromUtf8(pageTemplate), &context);

        response << content;
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
    const QHash<QString, QSharedPointer<QString>> &cookies = request.getHeader().getCookie();

    if (cookies.contains("ssid"))
    {
        QByteArray sessionId = (* cookies["ssid"].data()).toLatin1();
        QString email;
        m_sessionManager.findSession(sessionId, email);

        response.setStatusCode(200);
        response << "find user!\n" << email;
        response << "for session " << sessionId;
        response.finish();
    }
    else
    {
        response.setStatusCode(404);
        response << "can't find session, not logged in?\n";
        response.finish();
    }
}

void UserManagementUI::handleUserActivationUIGet(HttpRequest &request, HttpResponse &response)
{
    QByteArray pageTemplate;
    QString mimeType;
    const QHash<QString, QSharedPointer<QString>> &queries = request.getHeader().getQueries();

    if (queries.contains("email") && queries.contains("activation_code")
            && m_staticFileServer.getFileByAbsolutePath(m_templatePath % "/activation.html", pageTemplate, mimeType))
    {
        QVariantHash info;
        info["activation_code"] = (*queries["activation_code"].data());
        info["activation_email"] = (*queries["email"]);

        Mustache::Renderer renderer;
        Mustache::QtVariantContext context(info);

        QString content = renderer.render(QString::fromUtf8(pageTemplate), &context);

        response << content;
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
    QByteArray pageTemplate;
    QString mimeType;
    const QHash<QString, QSharedPointer<QString>> &queries = request.getHeader().getQueries();

    if (queries.contains("email") && queries.contains("reset_code")
            && m_staticFileServer.getFileByAbsolutePath(m_templatePath % "/reset_password_by_resetCode.html", pageTemplate, mimeType))
    {
        QVariantHash info;
        info["reset_code"] = *queries["reset_code"].data();
        info["reset_email"] = *queries["email"].data();

        Mustache::Renderer renderer;
        Mustache::QtVariantContext context(info);

        QString content = renderer.render(QString::fromUtf8(pageTemplate), &context);

        response << content;
        response.finish(mimeType);
    }
    else if ( m_staticFileServer.getFileByAbsolutePath(m_templatePath % "/reset_password_by_oldPassword.html", pageTemplate, mimeType))
    {
        QString email = "value=\"\" autofocus";
        if (queries.contains("email"))
        {
            email = "value=\"" % (*queries["email"].data()) % "\" readonly";
        }

        QVariantHash info;
        info["reset_email"] = email;

        Mustache::Renderer renderer;
        Mustache::QtVariantContext context(info);

        QString content = renderer.render(QString::fromUtf8(pageTemplate), &context);

        response << content;
        response.finish(mimeType);
    }
    else
    {
        response.setStatusCode(404);
        response << "can't find the file!\n";
        response.finish();
    }
}


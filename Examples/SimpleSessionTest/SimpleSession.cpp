#include "SimpleSession.h"
#include "LoggingManager.h"
#include "Swiftly.h"

SimpleSession::SimpleSession()
    :WebApp()
{

}

void SimpleSession::registerPathHandlers()
{
    addGetHandler("/signin","handleUserSigninGet");
    addGetHandler("/", "handleUserLoggedInGet");

}


void SimpleSession::handleUserSigninGet(HttpRequest &request,HttpResponse &response)
{
    QByteArray code;
    generateHashCode(code);
    response.setStatusCode(302);
    response.setSessionId(QString::fromUtf8(code));
    response.setHeader("Location", "/");
    response.finish();
}

void SimpleSession::handleUserLoggedInGet(HttpRequest &request, HttpResponse &response)
{
    response.setStatusCode(200);
    if (request.getHeader().hasCookies())
    {

        response << "logged in ";

        foreach (const QString &var, request.getHeader().getCookie())
        {
            response << var << " ";
        }
    }
    else
    {
        response << "logged in but no cookie/session";
    }
    response.finish();
}

#include "SimpleSession.h"
#include "../LoggingManager/LoggingManager/LoggingManager.h"
#include "Swiftly.h"

SimpleSession::SimpleSession()
    :WebApp()
{

}

void SimpleSession::registerPathHandlers()
{
    AddGetHandler("/", handleUserLoggedInGet);
    AddGetHandler("/api/signin", handleUserSigninGet);
}


void SimpleSession::handleUserSigninGet(HttpRequest &request,HttpResponse &response)
{
    QByteArray code;
    generateHashCode(code);
    response.setStatusCode(302);
    response.setSessionId(QString::fromUtf8(code));
    QSharedPointer<QString> rootPath(new QString("/"));
    response.setHeader("Location", rootPath);
    response.finish();
}

void SimpleSession::handleUserLoggedInGet(HttpRequest &request, HttpResponse &response)
{
    response.setStatusCode(200);
    if (request.getHeader().hasCookies())
    {

        response << "logged in ";

        foreach (const QSharedPointer<QString> &var, request.getHeader().getCookie())
        {
            response << (*var.data()) << " ";
        }
    }
    else
    {
        response << "logged in but no cookie/session";
    }
    response.finish();
}

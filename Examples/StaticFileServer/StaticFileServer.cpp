#include "StaticFileServer.h"
#include <QFile>

void StaticFileServer::registerPathHandlers()
{
   addGetHandler("/", "handleFileGet");
}

void StaticFileServer::handleFileGet(HttpRequest &request, HttpResponse &response)
{
    qDebug() << "-----------" << request.getHeader().getPath();

    QFile file(QString(".").append(request.getHeader().getPath()));
    if (file.exists())
    {
        if (file.open(QFile::ReadOnly))
        {
            response << file.readAll();
            response.finish(HttpResponse::BINARY);
        }
    }
    else
    {
        response.setStatusCode(404);
        response << "can't find the file!\n";
    }
}

#include "StaticServer.h"
#include <QFile>
#include <QFileInfo>

StaticServer::StaticServer():
    WebApp(),
    m_staticFileServer(QDir("/Users/shiyan/startmin"))
{}

void StaticServer::registerPathHandlers()
{
   addGetHandler("/", "handleFileGet");
}

void StaticServer::handleFileGet(HttpRequest &request, HttpResponse &response)
{
    qDebug() << "-----------%%%" << request.getHeader().getPath();

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

#include "StaticServer.h"
#include <QFile>
#include <QFileInfo>
#include <QSharedPointer>
#include <QString>

StaticServer::StaticServer():
    WebApp(),
    m_staticFileServer(QDir("/home/shiy/test"))
{}

void StaticServer::registerPathHandlers()
{
    AddGetHandler("/", handleFileGet);
}

void StaticServer::handleFileGet(HttpRequest &request, HttpResponse &response)
{
    qDebug() << "-----------%%%" << request.getHeader().getPath();

    bool compress = false;
    if (request.getHeader().getHeaderInfo().contains("Accept-Encoding") )
    {
        if (request.getHeader().getHeaderInfo()["Accept-Encoding"]->contains("gzip"))
        {
            compress = true;
        }
    }

    QByteArray fileContent;
    QString mimeType;
    if (m_staticFileServer.getFileByPath(request.getHeader().getPath(), fileContent, mimeType, StaticFileServer::FileType::UNSPECIFIED, true, compress))
    {
        response << fileContent;
        if (compress)
        {
            QSharedPointer<QString> gzipstr(new QString("gzip"));
            response.setHeader("Content-Encoding", gzipstr);
        }
        response.finish(mimeType);
    }
    else
    {
        response.setStatusCode(404);
        response << "can't find the file!\n";
        response.finish();
    }
}

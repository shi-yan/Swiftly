#include "StaticFileServer.h"
#include <QFile>
#include <QDebug>

//based on
//https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types
QMap<QString, QString> StaticFileServer::m_mimeTypeMap =
{{"txt" , "text/plain"},
 {"htm" , "text/html"},
 {"html", "text/html"},
 {"txt" , "text/css"},
 {"js"  , "text/javascript"},
 {"gif" , "image/gif"},
 {"png" , "image/png"},
 {"jpg" , "image/jpeg"},
 {"jpeg", "image/jpeg"},
 {"bmp" , "image/bmp"},
 {"webp", "image/webp"},
 {"midi", "audio/midi"},
 {"mpeg", "audio/mpeg"},
 {"mp3" , "audio/mp3"},
 {"wav" , "audio/wav"},
 {"webm", "video/webm"},
 {"ogg" , "audio/ogg"},
 {"xml" , "application/xml"},
 {"pdf" , "application/pdf"}};

StaticFileServer::StaticFileServer(const QDir &rootPath)
    :QObject(),
      m_rootDir(rootPath)
{
    if (!m_rootDir.exists())
    {
        m_rootDir = QDir("/Users/shiyan");
    }
    qDebug() << m_rootDir.canonicalPath();

}

StaticFileServer::StaticFileServer(const StaticFileServer &in)
    :QObject(),
      m_rootDir(in.m_rootDir)
{
    if (!m_rootDir.exists())
    {
        m_rootDir = QDir(".");
    }

}

bool StaticFileServer::getFileByPath(const QString &path, QByteArray &fileContent, QString &mimeType, FileType fileTypeHint)
{
    QFileInfo fileInfo(m_rootDir, path);

    qDebug() << fileInfo.canonicalFilePath() << fileInfo.absoluteFilePath();

    if (!fileInfo.isFile())
    {
        return false;
    }

    if (!fileInfo.exists())
    {
        return false;
    }

    if (fileInfo.canonicalFilePath().left(m_rootDir.canonicalPath().size()) != m_rootDir.canonicalPath())
    {
        return false;
    }

    QFile file(fileInfo.canonicalFilePath());
    if (file.open(QFile::ReadOnly))
    {
        fileContent = file.readAll();
        file.close();
    }

    if(m_mimeTypeMap.contains(fileInfo.suffix()))
    {
        mimeType = m_mimeTypeMap[fileInfo.suffix()];
    }
    else
    {
        //application/octet-stream
        //text/plain

        StaticFileServer::FileType fileType = fileTypeHint;

        if (fileType == StaticFileServer::FileType::UNSPECIFIED)
        {
            fileType = guessFileType(fileContent);
        }

        if (fileType == StaticFileServer::FileType::TEXT)
        {
            mimeType = "text/plain";
        }
        else if(fileType == StaticFileServer::FileType::BINARY)
        {
            mimeType = "application/octet-stream";
        }
        else
        {
            return false;
        }
    }

    return true;
}

StaticFileServer::FileType StaticFileServer::guessFileType(const QByteArray &fileContent)
{
    //Based on: https://stackoverflow.com/questions/277521/how-to-identify-the-file-content-as-ascii-or-binary
    const unsigned int maximumCheckLength = 100;
    unsigned int checkLength = (maximumCheckLength <= static_cast<unsigned int>(fileContent.size())) ? maximumCheckLength: fileContent.size();

    for (unsigned int i = 0; i < checkLength; ++i)
    {
        if (fileContent.data()[i] == 0)
        {
            return StaticFileServer::FileType::BINARY;
        }
    }

    return StaticFileServer::FileType::TEXT;
}

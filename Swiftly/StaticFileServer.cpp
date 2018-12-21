#include "StaticFileServer.h"
#include <QFile>
#include <QDebug>
#include <unistd.h>

//based on
//https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types
QHash<QString, QString> StaticFileServer::m_mimeTypeMap =
{{"txt" , "text/plain"},
 {"htm" , "text/html"},
 {"html", "text/html"},
 {"css" , "text/css"},
 {"js"  , "text/javascript"},
 {"gif" , "image/gif"},
 {"png" , "image/png"},
 {"jpg" , "image/jpeg"},
 {"jpeg", "image/jpeg"},
 {"bmp" , "image/bmp"},
 {"svg" , "image/svg+xml"},
 {"webp", "image/webp"},
 {"midi", "audio/midi"},
 {"mpeg", "audio/mpeg"},
 {"mp3" , "audio/mp3"},
 {"wav" , "audio/wav"},
 {"webm", "video/webm"},
 {"ogg" , "audio/ogg"},
 {"xml" , "application/xml"},
 {"pdf" , "application/pdf"}};

unsigned long long getAvailableSystemMemory()
{
    long pages = sysconf(_SC_AVPHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    unsigned long long avilableMem = pages * page_size;
    qDebug() << "Available Memory in MB: " << avilableMem / 1024 / 1024;
    return avilableMem ;
}

QMutex StaticFileServer::m_fileCacheMutex;
QCache<QString, StaticFileServer::FileCacheItem > StaticFileServer::m_fileCache(getHalfAvailableSystemMemory() / 2048);

StaticFileServer::FileCacheItem::FileCacheItem(const QFileInfo &fileInfo, const QByteArray &fileContent, const FileType fileType, const QString &mimeType)
    :m_fileInfo(fileInfo),
      m_fileContent(fileContent),
      m_fileType(fileType),
      m_mimeType(mimeType)
{}

StaticFileServer::StaticFileServer(const QDir &rootPath)
    :QObject(),
      m_rootDir(rootPath)
{
    if (!m_rootDir.exists())
    {
        m_rootDir = QDir(".");
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

bool StaticFileServer::getFileByPath(const QString &path, QByteArray &fileContent, QString &mimeType, FileType fileTypeHint, bool useCache) const
{
    QString filePath = m_rootDir.absolutePath().append(path);

    QFileInfo fileInfo(filePath);

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

    if (useCache)
    {
        StaticFileServer::m_fileCacheMutex.lock();
        if (!StaticFileServer::m_fileCache.contains(fileInfo.canonicalFilePath()))
        {
            m_fileCacheMutex.unlock();

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

            FileCacheItem *item = new FileCacheItem(fileInfo, fileContent, StaticFileServer::FileType::UNSPECIFIED, mimeType);
            qint64 sizeInKB = fileInfo.size() / 1024;

            if (sizeInKB < 1)
            {
                sizeInKB = 1;
            }

            StaticFileServer::m_fileCacheMutex.lock();

            StaticFileServer::m_fileCache.insert(fileInfo.absoluteFilePath(), item, (int) sizeInKB);
            qDebug() << "file" << fileInfo.absoluteFilePath() << "is in cache" << sizeInKB;

            StaticFileServer::m_fileCacheMutex.unlock();
        }
        else
        {
            FileCacheItem *item = StaticFileServer::m_fileCache[fileInfo.absoluteFilePath()];
            fileContent = item->m_fileContent;
            mimeType = item->m_mimeType;
            qDebug() << "file" << item->m_fileInfo.absoluteFilePath() << "is from cache";
            StaticFileServer::m_fileCacheMutex.unlock();
        }
    }
    else
    {
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
    }
    return true;

}


bool StaticFileServer::getFileByAbsolutePath(const QString &absolutePath, QByteArray &fileContent, QString &mimeType, FileType fileTypeHint, bool useCache) const
{
    QFileInfo fileInfo(absolutePath);

    qDebug() << fileInfo.canonicalFilePath() << fileInfo.absoluteFilePath();

    if (!fileInfo.isFile())
    {
        return false;
    }

    if (!fileInfo.exists())
    {
        return false;
    }

    if (useCache)
    {
        StaticFileServer::m_fileCacheMutex.lock();
        if (!StaticFileServer::m_fileCache.contains(fileInfo.canonicalFilePath()))
        {
            m_fileCacheMutex.unlock();

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

            FileCacheItem *item = new FileCacheItem(fileInfo, fileContent, StaticFileServer::FileType::UNSPECIFIED, mimeType);
            qint64 sizeInKB = fileInfo.size() / 1024;

            if (sizeInKB < 1)
            {
                sizeInKB = 1;
            }

            StaticFileServer::m_fileCacheMutex.lock();

            StaticFileServer::m_fileCache.insert(fileInfo.absoluteFilePath(), item, (int) sizeInKB);
            qDebug() << "file" << fileInfo.absoluteFilePath() << "is in cache" << sizeInKB;

            StaticFileServer::m_fileCacheMutex.unlock();
        }
        else
        {
            FileCacheItem *item = StaticFileServer::m_fileCache[fileInfo.absoluteFilePath()];
            fileContent = item->m_fileContent;
            mimeType = item->m_mimeType;
            qDebug() << "file" << item->m_fileInfo.absoluteFilePath() << "is from cache";
            StaticFileServer::m_fileCacheMutex.unlock();
        }
    }
    else
    {
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
    }
    return true;

}

StaticFileServer::FileType StaticFileServer::guessFileType(const QByteArray &fileContent) const
{
    //Based on:
    //https://stackoverflow.com/questions/277521/how-to-identify-the-file-content-as-ascii-or-binary
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

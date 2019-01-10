#ifndef STATICFILESERVER_H
#define STATICFILESERVER_H

#include <QObject>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QHash>
#include <QCache>
#include <QMutex>

class StaticFileServer : public QObject
{
    Q_OBJECT
private:
    QDir m_rootDir;

public:
    enum class FileType
    {
        TEXT,
        BINARY,
        UNSPECIFIED
    };

    class FileCacheItem
    {
    public:
        QFileInfo m_fileInfo;
        QByteArray m_fileContent;
        QByteArray m_fileGZipContent;
        FileType m_fileType;
        QString m_mimeType;
        QString m_md5;

    public:

        FileCacheItem(const QFileInfo &fileInfo, const QByteArray &fileContent, const FileType fileType, const QString &mimeType);

        unsigned int sizeInKB();
    };

    StaticFileServer(const QDir &root = QDir("."));
    StaticFileServer(const StaticFileServer &in);
    bool getFileByPath(const QString &path, QByteArray &fileContent, QString &mimeType, QString &md5, FileType fileTypeHint = FileType::UNSPECIFIED, bool useCache = true, bool compress = false) const;
    bool getFileByAbsolutePath(const QString &absolutePath, QByteArray &fileContent, QString &mimeType, QString &md5, FileType fileTypeHint = FileType::UNSPECIFIED, bool useCache = true, bool compress = false) const;
private:
    FileType guessFileType(const QByteArray &fileContent) const;
    static QHash<QString, QString> m_mimeTypeMap;
    static QMutex m_fileCacheMutex;
    static QCache<QString, FileCacheItem> m_fileCache;
};

#endif // STATICFILESERVER_H

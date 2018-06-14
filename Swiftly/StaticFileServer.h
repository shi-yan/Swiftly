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
        FileType m_fileType;
        QString m_mimeType;

    public:

        FileCacheItem(const QFileInfo &fileInfo, const QByteArray &fileContent, const FileType fileType, const QString &mimeType);
    };

    StaticFileServer(const QDir &root = QDir("."));
    StaticFileServer(const StaticFileServer &in);
    bool getFileByPath(const QString &path, QByteArray &fileContent, QString &mimeType, FileType fileTypeHint = FileType::UNSPECIFIED) const;
    bool getFileByAbsolutePath(const QString &absolutePath, QByteArray &fileContent, QString &mimeType, FileType fileTypeHint = FileType::UNSPECIFIED) const;
private:
    FileType guessFileType(const QByteArray &fileContent) const;
    static QHash<QString, QString> m_mimeTypeMap;
    static QMutex m_fileCacheMutex;
    static QCache<QString, FileCacheItem> m_fileCache;
};

#endif // STATICFILESERVER_H

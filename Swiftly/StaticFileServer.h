#ifndef STATICFILESERVER_H
#define STATICFILESERVER_H

#include <QObject>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QMap>


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
    StaticFileServer(const QDir &root = QDir("."));
    StaticFileServer(const StaticFileServer &in);
    bool getFileByPath(const QString &path, QByteArray &fileContent, QString &mimeType, FileType fileTypeHint = FileType::UNSPECIFIED);

private:
    FileType guessFileType(const QByteArray &fileContent);
    static QMap<QString, QString> m_mimeTypeMap;
};

#endif // STATICFILESERVER_H

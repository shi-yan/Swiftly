#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QTcpSocket>
#include <QByteArray>
#include <QSharedPointer>
#include "HttpRequest.h"
#include "HttpResponse.h"
#include <QTimer>
#include "http_parser.h"
#include <QUuid>
#include "PathTree.h"
#include <string>

class TcpSocket:public QTcpSocket
{
    Q_OBJECT


    QTimer m_suicideTimer;
    http_parser m_parser;
    http_parser_settings m_settings;
    QString m_consolePath;
    QString m_adminPassHash;
    QSharedPointer<PathTree> m_pathTree;
    int m_headerParseCounter = 0;
    int m_servedCount = 0;
    QByteArray m_buffer;
    const int m_timeout = 15*1000;

public:
    QSharedPointer<HttpRequest> m_request;
    QSharedPointer<HttpResponse> m_response;
    QUuid m_uuid;
    TcpSocket(QObject *parent, const QString &consolePath, const QString &adminPassHash, const QSharedPointer<PathTree> &pathTree);
    TcpSocket(const TcpSocket &in) = delete ;
    void operator=(const TcpSocket &in)= delete;
    ~TcpSocket() override;

    void setRawHeader(const QString &in);
    QString & getRawHeader();

    unsigned int getTotalBytes();
    unsigned int getBytesHaveRead();
    HttpHeader & getHeader();

    void setTotalBytes(unsigned int _totalBytes);

    void appendData(const char* buffer,unsigned int size);
    void appendData(const QByteArray &buffer);

    bool setSocketDescriptor(qintptr socketDescriptor, SocketState state = ConnectedState,
                                 OpenMode openMode = ReadWrite) override;
    void handleConsole(HttpRequest &request, HttpResponse &response);
    HttpRequest & getRequest()
    {
        return *m_request;
    }

    HttpResponse & getResponse()
    {
        return *m_response;
    }

    void setTimeout(int msec);

private slots:
    void disconnectSocket();
    void discardClient();
    void readClient();

signals:
    void deleteSocket(const QUuid &);
    void shutdown();
    void shutdownAll();
};

#endif // TCPSOCKET_H

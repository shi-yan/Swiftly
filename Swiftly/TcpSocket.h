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

class TcpSocket:public QTcpSocket
{
    Q_OBJECT

    bool m_isNew;

    QSharedPointer<HttpRequest> m_request;
    QSharedPointer<HttpResponse> m_response;
    QTimer m_suicideTimer;
    http_parser m_parser;
    QString m_consolePath;
    QString m_adminPassHash;
    QSharedPointer<PathTree> m_pathTree;

public:
    QUuid m_uuid;
    TcpSocket(const QString &consolePath, const QString &adminPassHash, const QSharedPointer<PathTree> &pathTree);
    TcpSocket(const TcpSocket &in) = delete ;
    void operator=(const TcpSocket &in)= delete;
    ~TcpSocket();

    void setRawHeader(const QString &in);
    QString & getRawHeader();

    unsigned int getTotalBytes();
    unsigned int getBytesHaveRead();
    HttpHeader & getHeader();
    bool isEof();
    void notNew();

    bool isNewSocket();
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
};

#endif // TCPSOCKET_H

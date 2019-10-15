#ifndef WORKER_H
#define WORKER_H

#include <QThread>
#include <QHash>

#include "PathTree.h"
#include "WebApp.h"
#include <QSemaphore>
#include <QHash>
#include <QUuid>

class WorkerSocketWatchDog;
class IncomingConnectionQueue;

class Worker : public QThread
{
    Q_OBJECT

    QString m_name;

    QHash<int, WebApp*> m_webAppTable;
    QSharedPointer<PathTree> m_pathTree;
    QHash<QUuid, TcpSocket*> m_sockets;
    QString m_consolePath;
    QString m_adminPassHash;

    bool parseFormData(const QString &contentTypeString, const QByteArray &_body, QHash<QString,QByteArray> &formData);

public:
    void run();
    Worker(const QString &name, const QString &consolePath = QString(), const QString &adminPassHash = QString());
    void registerWebApps(QVector<int> &webAppClassIDs);
    void waitForIdle();
    ~Worker();
    qintptr getSocket();
    void addNewSocket(qintptr socket);

public slots:
    void newSocket(qintptr socketid);
    void deleteSocket(const QUuid &uuid);
    void shutdownAll();

signals:
    void shutdown();
    void receivedNewSocket(qintptr socketId);

private:
    void handleConsole(HttpRequest &request, HttpResponse &response);
};

#endif // WORKER_H

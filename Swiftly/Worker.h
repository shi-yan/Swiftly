#ifndef WORKER_H
#define WORKER_H

#include <QThread>
#include <QHash>
#include "http_parser.h"
#include "PathTree.h"
#include "WebApp.h"
#include <QSemaphore>

class WorkerSocketWatchDog;
class IncomingConnectionQueue;

class Worker : public QThread
{
    Q_OBJECT

    QString m_name;
    http_parser m_parser;
    QHash<int, WebApp*> m_webAppTable;
    QSharedPointer<PathTree> m_pathTree;
    QSemaphore m_idleSemaphore;
    WorkerSocketWatchDog *m_socketWatchDog;
    IncomingConnectionQueue *m_incomingConnectionQueue;
    QString m_consolePath;
    QString m_adminPassHash;

    bool parseFormData(const QString &contentTypeString, const QByteArray &_body, QHash<QString,QByteArray> &formData);

public:
    void run();
    Worker(const QString &name, IncomingConnectionQueue *connectionQueue, const QString &consolePath = QString(), const QString &adminPassHash = QString());
    void registerWebApps(QVector<int> &webAppClassIDs);
    void waitForIdle();
    ~Worker();
    qintptr getSocket();

public slots:
    void readClient();
    void discardClient();
    void newSocket(qintptr socketid);
    void watchDogFinished();

signals:
    void shutdown();

private:
    void handleConsole(HttpRequest &request, HttpResponse &response);
};

#endif // WORKER_H

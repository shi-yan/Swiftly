#ifndef WORKER_H
#define WORKER_H

#include <QtCore/QThread>
#include <QtCore/QMap>
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
    QMap<int, WebApp*> m_webAppTable;
    PathTree m_pathTree;
    QSemaphore m_idleSemaphore;
    WorkerSocketWatchDog *m_socketWatchDog;
    IncomingConnectionQueue *m_incomingConnectionQueue;

    bool parseFormData(const QString &contentTypeString, const QByteArray &_body, QMap<QString,QByteArray> &formData);

public:
    void run();
    Worker(const QString &name, IncomingConnectionQueue *connectionQueue);
    void registerWebApps(QVector<int> &webAppClassIDs);
    void waitForIdle();
    ~Worker();
    qintptr getSocket();

public slots:
    void readClient();
    void discardClient();
    void newSocket(qintptr socketid);
};

#endif // WORKER_H

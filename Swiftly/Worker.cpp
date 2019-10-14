#include "Worker.h"
#include <QDebug>
#include "TcpSocket.h"
#include <QDateTime>
#include "HttpHeader.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "PathTree.h"
#include "IncomingConnectionQueue.h"
#include <QtCore/QCoreApplication>
#include "LoggingManager.h"
#include <QHostAddress>
#include <QCryptographicHash>
#include "AdminPageContent.h"

Worker::Worker(const QString &name, const QString &consolePath, const QString &adminPassHash)
    :QThread(),
      m_name(name),
      m_webAppTable(),
      m_pathTree(new PathTree()),
      m_consolePath(consolePath),
      m_adminPassHash(adminPassHash)
{
    connect(this,SIGNAL(receivedNewSocket(qintptr)), this,SLOT(newSocket(qintptr)));
}

Worker::~Worker()
{
    qDebug() << "deleted!" ;
}

void Worker::newSocket(qintptr socket)
{
    TcpSocket* s = new TcpSocket(m_consolePath, m_adminPassHash, m_pathTree);

    s->setSocketDescriptor(socket);

    m_sockets[s->m_uuid] = QSharedPointer<TcpSocket>(s);

    connect(s, SIGNAL(deleteSocket(const QUuid &)), this, SLOT(deleteSocket(const QUuid &)));

#ifndef NO_LOG
    sLog() << m_name << " receive a new request from ip:" << s->peerAddress().toString();
#endif
}

void Worker::deleteSocket(const QUuid &uuid)
{
    m_sockets.remove(uuid);
}

void Worker::registerWebApps(QVector<int> &webAppClassIDs)
{
    for(int i=0;i<webAppClassIDs.count();++i)
    {
        WebApp *app= static_cast<WebApp*> (QMetaType::create(static_cast<int>(webAppClassIDs[i]), nullptr));

        m_webAppTable[webAppClassIDs[i]]=app;

        app->setPathTree(m_pathTree);

        app->registerPathHandlers();

        app->init();
    }
}

void Worker::run()
{
#ifndef NO_LOG
    qDebug() << m_name<<"'s thread id"<<thread()->currentThreadId();
    sLog() << m_name << "'s thread id" << thread()->currentThreadId();
#endif
    exec();
}

void Worker::addNewSocket(qintptr socket)
{
    emit receivedNewSocket(socket);
}

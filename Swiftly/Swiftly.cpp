#include "Swiftly.h"
#include "HttpHeader.h"
#include "Worker.h"
#include "IncomingConnectionQueue.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include <sodium.h>
#include "ReCAPTCHAVerifier.h"
#include <QSettings>
#include "SettingsManager.h"

HttpServer::HttpServer(QObject* parent )
    : QTcpServer(parent), 
      m_connectionCount(0),
      m_disabled(false),
      m_incomingConnectionQueue(nullptr),
      m_webAppSet()

{
    qRegisterMetaType<qintptr>("qintptr");
    qRegisterMetaType<HttpRequest>("HttpRequest");
    qRegisterMetaType<HttpResponse>("HttpResponse");
    //setMaxPendingConnections(2000);

    m_incomingConnectionQueue = new IncomingConnectionQueue(this);

}

HttpServer::~HttpServer()
{
}

void HttpServer::incomingConnection(qintptr socket)
{
    if (m_disabled)
        return;

    m_incomingConnectionQueue->addSocket(socket);

    qDebug()<<"New Connection!" << serverAddress().toString() << socket;

    m_connectionCount++;

    qDebug() << "connection:" << m_connectionCount;
}

void HttpServer::start(int numOfWorkers, quint16 port)
{
    if(sodium_init()!=0)
    {
        qDebug() << "sodium problem";
    }

    SettingsManager::getSingleton().init();

    if(SettingsManager::getSingleton().has("reCAPTCHA/secret"))
    {
        qDebug() << "has reCAPTCHA settings";
        QString secret = SettingsManager::getSingleton().get("reCAPTCHA/secret").toString();
        ReCAPTCHAVerifier::getSingleton().init(secret);
    }

    qDebug()<<"Need to create"<<numOfWorkers<<"workers";

    if(numOfWorkers<1)
        numOfWorkers=1;

    for(int i=0;i<numOfWorkers;++i)
    {
        Worker *aWorker=new Worker(QString("worker %1").arg(i), m_incomingConnectionQueue);
        aWorker->registerWebApps(m_webAppSet);
        aWorker->start();
        aWorker->moveToThread(aWorker);
        m_workerPool.push_back(aWorker);
    }

    listen(QHostAddress::Any, port);

    qDebug()<<"Start listening! main ThreadId"<<thread()->currentThreadId();
}

void HttpServer::Shutdown()
{
    m_disabled = true;
}

void HttpServer::pause()
{
    m_disabled = true;
}

void HttpServer::resume()
{
    m_disabled = false;
}

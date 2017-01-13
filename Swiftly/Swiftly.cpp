#include "Swiftly.h"
#include "HttpHeader.h"
#include "Worker.h"
#include "IncommingConnectionQueue.h"
#include "HttpRequest.h"
#include "HttpResponse.h"

HttpServer::HttpServer(QObject* parent )
    : QTcpServer(parent), 
    disabled(false),
    connectionCount(0)
{
    qRegisterMetaType<HttpRequest>("HttpRequest");
    qRegisterMetaType<HttpResponse>("HttpResponse");
    //setMaxPendingConnections(2000);
}

HttpServer::~HttpServer()
{
}

void HttpServer::incomingConnection(qintptr socket)
{
    if (disabled)
        return;

    InCommingConnectionQueue::getSingleton().insertATask(socket);

    qDebug()<<"New Connection!"<<socket;

    connectionCount++;
    qDebug() << "connection:" << connectionCount;
}

void HttpServer::start(int numOfWorkers,quint16 port)
{
    qDebug()<<"Need to create"<<numOfWorkers<<"workers";

    InCommingConnectionQueue::getSingleton();

    if(numOfWorkers<1)
        numOfWorkers=1;


    for(int i=0;i<numOfWorkers;++i)
    {
        Worker *aWorker=new Worker(QString("worker %1").arg(i));
        aWorker->registerWebApps(webAppSet);
        aWorker->start();
        aWorker->moveToThread(aWorker);
    }

    listen(QHostAddress::Any, port);

    qDebug()<<"Start listening! main ThreadId"<<thread()->currentThreadId();
}

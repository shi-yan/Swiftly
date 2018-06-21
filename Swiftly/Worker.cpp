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
#include "WorkerSocketWatchDog.h"
#include "LoggingManager.h"
#include <QHostAddress>

int onMessageBegin(http_parser *)
{
   // qDebug()<<"Parse Message Begin";
    return 0;
}

int onPath(http_parser *parser, const char *p,size_t len)
{
    QByteArray buffer(p,len);
    //qDebug()<<"onPath:"<<QString(buffer);

    ((TcpSocket*)parser->data)->getHeader().setPath(QString(buffer));

    return 0;
}

int onQueryString(http_parser *parser, const char *p,size_t len)
{
    QByteArray buffer(p,len);
    // qDebug()<<"onQueryString:"<<QString(buffer);
    ((TcpSocket*)parser->data)->getHeader().setQueryString(QString(buffer));
    return 0;
}

int onUrl(http_parser *parser, const char *p,size_t len)
{
    QByteArray buffer(p,len);
    //qDebug()<<"onUrl:"<<QString(buffer);
    ((TcpSocket*)parser->data)->getHeader().setUrl(QString(buffer));

    http_parser_url *u = (http_parser_url*)malloc(sizeof(http_parser_url));
    http_parser_parse_url(p,len,1,u);

    if (u->field_set & (1 << UF_SCHEMA))
    {
        QString string(QByteArray(&p[u->field_data[UF_SCHEMA].off], u->field_data[UF_SCHEMA].len));
        //qDebug() << "UF_SCHEMA" << string;
    }

    if (u->field_set & (1 << UF_HOST))
    {
        QString string(QByteArray(&p[u->field_data[UF_HOST].off], u->field_data[UF_HOST].len));
        //qDebug() << "UF_HOST" << string;
    }

    if (u->field_set & (1 << UF_PORT))
    {
        QString string(QByteArray(&p[u->field_data[UF_PORT].off], u->field_data[UF_PORT].len));
        //qDebug() << "UF_PORT" << string;
    }

    if (u->field_set & (1<<UF_PATH))
    {
        QString string(QByteArray(&p[u->field_data[UF_PATH].off], u->field_data[UF_PATH].len));
        ((TcpSocket*)parser->data)->getHeader().setPath(string);
        //qDebug() << "UF_PATH" << string;
    }

    if (u->field_set & (1<<UF_QUERY))
    {
        QString string(QByteArray(&p[u->field_data[UF_QUERY].off], u->field_data[UF_QUERY].len));
        ((TcpSocket*)parser->data)->getHeader().setQueryString(string);
        //qDebug() << "UF_QUERY" << string;
    }

    if (u->field_set & (1<<UF_FRAGMENT))
    {
        QString string(QByteArray(&p[u->field_data[UF_FRAGMENT].off], u->field_data[UF_FRAGMENT].len));
        //qDebug() << "UF_FRAGMENT" << string;
    }

    if (u->field_set & (1<<UF_USERINFO))
    {
        QString string(QByteArray(&p[u->field_data[UF_USERINFO].off], u->field_data[UF_USERINFO].len));
        //qDebug() << "UF_USERINFO" << string;
    }

    delete(u);
    return 0;
}

int onFragment(http_parser *parser, const char *p,size_t len)
{
    QByteArray buffer(p,len);
    //qDebug()<<"onFragment:"<<QString(buffer);

    ((TcpSocket*)parser->data)->getHeader().setFragment(QString(buffer));
    return 0;
}

int onHeaderField(http_parser *parser, const char *p,size_t len)
{
    QByteArray buffer(p,len);
    //qDebug()<<"onHeaderField:"<<QString(buffer);
    ((TcpSocket*)parser->data)->getHeader().setCurrentHeaderField(QString(buffer));
    return 0;
}

int onHeaderValue(http_parser *parser, const char *p,size_t len)
{
    QByteArray buffer(p,len);
    QSharedPointer<QString> headerValue(new QString(buffer));
    ((TcpSocket*)parser->data)->getHeader().addHeaderInfo(headerValue);
    return 0;
}

int onHeadersComplete(http_parser *parser)
{   
    TcpSocket *socket = (TcpSocket*)parser->data;
#ifndef NO_LOG
    sLog(LogEndpoint::LogLevel::DEBUG) << " === parsed header ====";
    const QHash<QString, QSharedPointer<QString>> &headers = socket->getHeader().getHeaderInfo();

    QHash<QString, QSharedPointer<QString>>::const_iterator i = headers.constBegin();
    while (i != headers.constEnd())
    {
        sLog(LogEndpoint::LogLevel::DEBUG) << i.key() << *(i.value().data());
        ++i;
    }
    sLog(LogEndpoint::LogLevel::DEBUG) << " === ============= ====";
    sLogFlush();
#endif
    QWeakPointer<QString> host = socket->getHeader().getHeaderInfo("Host");

    if (!host.isNull())
    {
        socket->getHeader().setHost(*host.data());
    }

    return 0;
}

int onBody(http_parser *parser, const char *p,size_t len)
{
     // QByteArray buffer(p,len);
    //  qDebug()<<"onBody:"<<QString(buffer);
    ((TcpSocket*)parser->data)->appendData(p,len);
    return 0;
}

int onMessageComplete(http_parser *)
{
    //qDebug()<<"Parse Message Complete";
    return 0;
}

Worker::Worker(const QString &name, IncomingConnectionQueue *connectionQueue)
    :QThread(),
      m_name(name),
      m_parser(),
      m_webAppTable(),
      m_pathTree(new PathTree()),
      m_idleSemaphore(),
      m_socketWatchDog(nullptr),
      m_incomingConnectionQueue(connectionQueue)
{
}

Worker::~Worker()
{
    if (m_socketWatchDog)
    {
        delete m_socketWatchDog;
    }
}

void Worker::newSocket(qintptr socket)
{
    //qDebug() << m_name << " is handling a new request; thread id" << thread()->currentThreadId();

    TcpSocket* s = new TcpSocket(this);
    s->m_id = rand();
    connect(s, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(s, SIGNAL(disconnected()), this, SLOT(discardClient()));
    s->setSocketDescriptor(socket);
#ifndef NO_LOG
    sLog() << m_name << " receive a new request from ip:" << s->peerAddress().toString();
#endif
}

void Worker::readClient()
{
    //if (disabled)
    //    return;

    // This slot is called when the client sent data to the server. The
    // server looks if it was a get request and sends a very simple HTML
    // document back.
    TcpSocket* socket = (TcpSocket*)sender();

    if(socket->bytesAvailable())
    {
        if ( socket->isNewSocket())
        {
            QByteArray incomingContent=socket->readAll();

            http_parser_settings settings;

            settings. on_message_begin=onMessageBegin;
            settings. on_url=onUrl;
            settings. on_header_field=onHeaderField;
            settings. on_header_value=onHeaderValue;
            settings. on_headers_complete=onHeadersComplete;
            settings. on_body=onBody;
            settings. on_message_complete=onMessageComplete;

            http_parser_init(&m_parser,HTTP_REQUEST);

            m_parser.data = socket;

            size_t nparsed = http_parser_execute(&m_parser,&settings,incomingContent.constData(),incomingContent.count());

            if(m_parser.upgrade)
            {
                qDebug()<<"upgrade";
            }
            else if(nparsed!=(size_t)incomingContent.count())
            {
                qDebug()<<"nparsed:"<<nparsed<<"buffer size:"<<incomingContent.count();
            }
            else
            {
                //qDebug()<<"parsing seems to be succeed!";
            }

            socket->setRawHeader(incomingContent);

            bool isBodySizeOK=false;
            unsigned int bodySize = 0;
            QSharedPointer<QString> contentLength = socket->getHeader().getHeaderInfo("Content-Length");

            if (!contentLength.isNull())
            {
                bodySize = contentLength->toUInt(&isBodySizeOK);

                if(isBodySizeOK==false)
                {
                    bodySize=0;
                }
            }

            socket->setTotalBytes(bodySize);

            socket->notNew();
        }
        else
        {
            qDebug()<<"socket size:"<<socket->getTotalBytes()<<"current Size:"<<socket->getBytesHaveRead();
            QByteArray incomingContent=socket->readAll();
            socket->appendData(incomingContent);
        }

        if(socket->isEof())
        {
            PathTreeNode::HttpVerb handlerType;

            if(m_parser.method==HTTP_GET)
            {
                socket->getHeader().setHttpMethod(HttpHeader::HttpMethod::HTTP_GET);
                handlerType=PathTreeNode::GET;
            }
            else if(m_parser.method==HTTP_POST)
            {
                socket->getHeader().setHttpMethod(HttpHeader::HttpMethod::HTTP_POST);
                handlerType=PathTreeNode::POST;
            }
            else
            {
                qDebug()<<"not get and post"<<socket->atEnd()<<socket->bytesAvailable()<<socket->ConnectedState;
                socket->close();
                return;
            }

            socket->getRequest().processCookies();
            socket->getRequest().parseFormData();

            //qDebug() << "path" << socket->getRequest().getHeader().getPath();
#ifndef NO_LOG
            sLog() << "handle request:" << socket->getRequest().getHeader().getPath();
            qDebug() << "handle request:" << socket->getRequest().getHeader().getPath();
#endif
            const std::function<void (HttpRequest &, HttpResponse &)> &th = m_pathTree->getTaskHandlerByPath(socket->getRequest().getHeader().getPath(), handlerType);

            if(th)
            {
                th(socket->getRequest(), socket->getResponse());
                socket->getResponse().finish();
            }
            else
            {
#ifndef NO_LOG
                qDebug()<<"empty task handler!" << socket->getRequest().getHeader().getPath() << ";" <<handlerType;
                sLog()<<"empty task handler!" << socket->getRequest().getHeader().getPath() << ";" <<handlerType;
#endif
                socket->getResponse().setStatusCode(404);
                socket->getResponse().finish();
            }

            socket->waitForBytesWritten();
            socket->close();
        }
        else
        {
            qDebug()<<"socket size:"<<socket->getTotalBytes()<<"current size:"<<socket->getBytesHaveRead();
            return;
        }
    }
}


void Worker::registerWebApps(QVector<int> &webAppClassIDs)
{
    for(int i=0;i<webAppClassIDs.count();++i)
    {
        WebApp *app= (WebApp*) QMetaType::create((int)webAppClassIDs[i], (const void *)0);

        m_webAppTable[webAppClassIDs[i]]=app;

        app->setPathTree(m_pathTree);

        app->registerPathHandlers();

        app->init();
    }
}

void Worker::discardClient()
{
    TcpSocket* socket = (TcpSocket*)sender();
#ifndef NO_LOG
    qDebug() << "thread id" << thread()->currentThreadId();
    qDebug() << "release socket" << socket << socket->m_id;
    //qDebug() << "finish serving client inside" << m_name;
    sLog() << m_name << "finished request.";
    sLogFlush();
#endif
    socket->deleteLater();
    m_idleSemaphore.release();
}

void Worker::run()
{
#ifndef NO_LOG
    qDebug() << m_name<<"'s thread id"<<thread()->currentThreadId();
    sLog() << m_name << "'s thread id" << thread()->currentThreadId();
#endif
    m_socketWatchDog = new WorkerSocketWatchDog(this);
    m_socketWatchDog->start();
    m_socketWatchDog->setPriority(QThread::HighestPriority);
    exec();
}

void Worker::waitForIdle()
{
    m_idleSemaphore.acquire();
}

qintptr Worker::getSocket()
{
    return m_incomingConnectionQueue->getSocket();
}


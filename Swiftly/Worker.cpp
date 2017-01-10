#include "Worker.h"
#include <QDebug>
#include "TcpSocket.h"
#include <QDateTime>
#include "HttpHeader.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "PathTree.h"
#include "IncommingConnectionQueue.h"
#include <QtCore/QCoreApplication>

int onMessageBegin(http_parser *)
{
   // qDebug()<<"Parse Message Begin";
    return 0;
}

int onPath(http_parser *parser, const char *p,size_t len)
{
    QByteArray buffer(p,len);
  //  qDebug()<<"onPath:"<<QString(buffer);

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
    qDebug()<<"onUrl:"<<QString(buffer);
    ((TcpSocket*)parser->data)->getHeader().setUrl(QString(buffer));

    http_parser_url *u = (http_parser_url*)malloc(sizeof(http_parser_url));
    http_parser_parse_url(p,len,1,u);

    if (u->field_set & (1 << UF_SCHEMA))
    {
        QString string(QByteArray(&p[u->field_data[UF_SCHEMA].off], u->field_data[UF_SCHEMA].len));

        qDebug() << "UF_SCHEMA" << string;
    }

    if (u->field_set & (1 << UF_HOST))
    {

        QString string(QByteArray(&p[u->field_data[UF_HOST].off], u->field_data[UF_HOST].len));

        qDebug() << "UF_HOST" << string;

    }

    if (u->field_set & (1 << UF_PORT))
    {


        QString string(QByteArray(&p[u->field_data[UF_PORT].off], u->field_data[UF_PORT].len));

        qDebug() << "UF_PORT" << string;
    }

    if (u->field_set & (1<<UF_PATH))
    {

        QString string(QByteArray(&p[u->field_data[UF_PATH].off], u->field_data[UF_PATH].len));
        ((TcpSocket*)parser->data)->getHeader().setPath(string);

        qDebug() << "UF_PATH" << string;
    }



    if (u->field_set & (1<<UF_QUERY))
    {

        QString string(QByteArray(&p[u->field_data[UF_QUERY].off], u->field_data[UF_QUERY].len));
        ((TcpSocket*)parser->data)->getHeader().setQueryString(string);

        qDebug() << "UF_QUERY" << string;
    }


    if (u->field_set & (1<<UF_FRAGMENT))
    {

        QString string(QByteArray(&p[u->field_data[UF_FRAGMENT].off], u->field_data[UF_FRAGMENT].len));

        qDebug() << "UF_FRAGMENT" << string;
    }

    if (u->field_set & (1<<UF_USERINFO))
    {

        QString string(QByteArray(&p[u->field_data[UF_USERINFO].off], u->field_data[UF_USERINFO].len));

        qDebug() << "UF_USERINFO" << string;
    }

    delete(u);

    return 0;
}

int onFragment(http_parser *parser, const char *p,size_t len)
{
    QByteArray buffer(p,len);
  //  qDebug()<<"onFragment:"<<QString(buffer);

    ((TcpSocket*)parser->data)->getHeader().setFragment(QString(buffer));
    return 0;
}

int onHeaderField(http_parser *parser, const char *p,size_t len)
{
    QByteArray buffer(p,len);
    qDebug()<<"onHeaderField:"<<QString(buffer);
    ((TcpSocket*)parser->data)->getHeader().setCurrentHeaderField(QString(buffer));
    return 0;
}

int onHeaderValue(http_parser *parser, const char *p,size_t len)
{
    QByteArray buffer(p,len);
  //  qDebug()<<"onHeaderValue:"<<QString(buffer);
    ((TcpSocket*)parser->data)->getHeader().addHeaderInfo(QString(buffer));
    return 0;
}

int onHeadersComplete(http_parser *parser)
{
    ((TcpSocket*)parser->data)->getHeader().setHost(((TcpSocket*)parser->data)->getHeader().getHeaderInfo("Host"));
  // qDebug()<<"Parse Header Complete";
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
    qDebug()<<"Parse Message Complete";
    return 0;
}

Worker::Worker(const QString _name):inHandlingARequest(false),pathTree(),webAppTable()
{
   workerName=_name;
}

void Worker::newSocket(int socket)
{
    qDebug()<<workerName<<" is handling a request; thread id"<<thread()->currentThreadId();
    TcpSocket* s = new TcpSocket(this);
    connect(s, SIGNAL(readyRead()), this    , SLOT(readClient()));
    connect(s, SIGNAL(disconnected()), this, SLOT(discardClient()));
    inHandlingARequest=true;
    s->setSocketDescriptor(socket);
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
            QByteArray inCommingContent=socket->readAll();

            http_parser_settings settings;

            settings. on_message_begin=onMessageBegin;
            settings. on_url=onUrl;
            settings. on_header_field=onHeaderField;
            settings. on_header_value=onHeaderValue;
            settings. on_headers_complete=onHeadersComplete;
            settings. on_body=onBody;
            settings. on_message_complete=onMessageComplete;

            http_parser_init(&parser,HTTP_REQUEST);

            parser.data = socket;

            size_t nparsed = http_parser_execute(&parser,&settings,inCommingContent.constData(),inCommingContent.count());

            if(parser.upgrade)
            {
                qDebug()<<"upgrade";
            }
            else if(nparsed!=(size_t)inCommingContent.count())
            {
                qDebug()<<"nparsed:"<<nparsed<<"buffer size:"<<inCommingContent.count();
            }
            else
            {
                qDebug()<<"parsing seems to be succeed!";
            }

            socket->setRawHeader(inCommingContent);

            bool isBodySizeOK=false;
            unsigned int bodySize=socket->getHeader().getHeaderInfo("Content-Length").toUInt(&isBodySizeOK);

            if(isBodySizeOK==false)
            {
                bodySize=0;
            }

            socket->setTotalBytes(bodySize);

            socket->notNew();
        }
        else
        {
            qDebug()<<"socket size:"<<socket->getTotalBytes()<<"current Size:"<<socket->getBytesHaveRead();
            QByteArray inCommingContent=socket->readAll();
            socket->appendData(inCommingContent);
        }

        if(socket->isEof())
        {
            PathTreeNode::TaskHandlerType handlerType;

            if(parser.method==HTTP_GET)
            {
                socket->getHeader().setHttpMethod(HttpHeader::HTTP_GET);
                handlerType=PathTreeNode::GET;
            }
            else if(parser.method==HTTP_POST)
            {
                socket->getHeader().setHttpMethod(HttpHeader::HTTP_POST);
                handlerType=PathTreeNode::POST;
            }
            else
            {
                qDebug()<<"not get and post"<<socket->atEnd()<<socket->bytesAvailable()<<socket->ConnectedState;
                //socket->close();
                return;
            }
/*
            if(socket->getHeader().getPath().left(6)=="/file/")
            {
                qDebug()<<"serving binary!";
                FILE *file;
                char *buffer;
                unsigned long fileLen;

                file=fopen((QString("file/")+socket->getHeader().getPath().right(socket->getHeader().getPath().count()-6)).toStdString().c_str(),"rb");

                if(!file)
                {
                    qDebug()<<"unable to open file.";
                    socket->close();
                    return;
                }

                fseek(file,0,SEEK_END);
                fileLen=ftell(file);
                fseek(file,0,SEEK_SET);

                buffer=(char *)malloc(fileLen+1);

                if(!buffer)
                {
                    qDebug()<<"Memory error!";
                    fclose(file);
                    socket->close();
                    return;
                }

                fread(buffer,fileLen,1,file);

                fclose(file);

                QDataStream os(socket);
                os.writeRawData(buffer,fileLen);

                free(buffer);

                socket->close();
            }
            else*/
            {
                socket->getRequest().processCookies();
                socket->getRequest().parseFormData();

                qDebug() << "path" << socket->getRequest().getHeader().getPath();
                const TaskHandler *th=pathTree.getTaskHandlerByPath(socket->getRequest().getHeader().getPath(),handlerType);

                if(th)
                {
                    if(!th->isEmpty())
                    {
                        if(th->invoke(socket->getRequest(),socket->getResponse()))
                        {
                            socket->getResponse().finish();

                            qDebug()<<"invoke successful!";
                        }
                        else
                        {
                            qDebug()<<"invoke unsuccessful!";
                        }
                    }
                    else
                    {

                        qDebug()<<"no task handler!";
                        socket->getResponse().setStatusCode(404);
                        socket->getResponse().finish();
                    }
                }
                else
                {
                    qDebug()<<"empty task handler!" << socket->getRequest().getHeader().getPath() << ";" <<handlerType;
                    socket->getResponse().setStatusCode(404);
                    socket->getResponse().finish();
                }
                socket->waitForBytesWritten();
                socket->close();
            }
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

        webAppTable[webAppClassIDs[i]]=app;

        app->setPathTree(&pathTree);

        app->registerPathHandlers();

        app->init();
    }
}

void Worker::discardClient()
{
    TcpSocket* socket = (TcpSocket*)sender();
    socket->deleteLater();
    inHandlingARequest=false;
    qDebug()<<"discard client inside worker";
}

void Worker::run()
{
    qDebug()<<workerName<<"'s thread id"<<thread()->currentThreadId();

    forever
    {
        if(!inHandlingARequest)
        {
            int socket=InCommingConnectionQueue::getSingleton().getATask();

            if(socket!=-1)
            {
                newSocket(socket);
            }
        }

        QCoreApplication::processEvents();
    }

    exec();
}

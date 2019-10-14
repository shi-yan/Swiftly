#include "TcpSocket.h"
#include "LoggingManager.h"
#include "PathTreeNode.h"
#include <QCryptographicHash>
#include "AdminPageContent.h"

int onMessageBegin(http_parser *)
{
    return 0;
}

int onPath(http_parser *parser, const char *p,size_t len)
{
    QByteArray buffer(p,len);
    static_cast<TcpSocket*>(parser->data)->getHeader().setPath(QString(buffer));
    return 0;
}

int onQueryString(http_parser *parser, const char *p,size_t len)
{
    QByteArray buffer(p,len);
    static_cast<TcpSocket*>(parser->data)->getHeader().setQueryString(QString(buffer));
    return 0;
}

int onUrl(http_parser *parser, const char *p,size_t len)
{
    QByteArray buffer(p,len);
    static_cast<TcpSocket*>(parser->data)->getHeader().setUrl(QString(buffer));

    http_parser_url *u = static_cast<http_parser_url*>(malloc(sizeof(http_parser_url)));
    http_parser_parse_url(p,len,1,u);

    if (u->field_set & (1 << UF_SCHEMA))
    {
        QString string(QByteArray(&p[u->field_data[UF_SCHEMA].off], u->field_data[UF_SCHEMA].len));
    }

    if (u->field_set & (1 << UF_HOST))
    {
        QString string(QByteArray(&p[u->field_data[UF_HOST].off], u->field_data[UF_HOST].len));
    }

    if (u->field_set & (1 << UF_PORT))
    {
        QString string(QByteArray(&p[u->field_data[UF_PORT].off], u->field_data[UF_PORT].len));
    }

    if (u->field_set & (1<<UF_PATH))
    {
        QString string(QByteArray(&p[u->field_data[UF_PATH].off], u->field_data[UF_PATH].len));
        static_cast<TcpSocket*>(parser->data)->getHeader().setPath(string);
    }

    if (u->field_set & (1<<UF_QUERY))
    {
        QString string(QByteArray(&p[u->field_data[UF_QUERY].off], u->field_data[UF_QUERY].len));
        static_cast<TcpSocket*>(parser->data)->getHeader().setQueryString(string);
    }

    if (u->field_set & (1<<UF_FRAGMENT))
    {
        QString string(QByteArray(&p[u->field_data[UF_FRAGMENT].off], u->field_data[UF_FRAGMENT].len));
    }

    if (u->field_set & (1<<UF_USERINFO))
    {
        QString string(QByteArray(&p[u->field_data[UF_USERINFO].off], u->field_data[UF_USERINFO].len));
    }

    free(u);
    return 0;
}

int onFragment(http_parser *parser, const char *p,size_t len)
{
    QByteArray buffer(p,len);
    //qDebug()<<"onFragment:"<<QString(buffer);

    static_cast<TcpSocket*>(parser->data)->getHeader().setFragment(QString(buffer));
    return 0;
}

int onHeaderField(http_parser *parser, const char *p,size_t len)
{
    QByteArray buffer(p,len);
    //qDebug()<<"onHeaderField:"<<QString(buffer);
    static_cast<TcpSocket*>(parser->data)->getHeader().setCurrentHeaderField(QString(buffer));
    return 0;
}

int onHeaderValue(http_parser *parser, const char *p,size_t len)
{
    QByteArray buffer(p,len);
    QSharedPointer<QString> headerValue(new QString(buffer));
    static_cast<TcpSocket*>(parser->data)->getHeader().addHeaderInfo(headerValue);
    return 0;
}

int onHeadersComplete(http_parser *parser)
{
    TcpSocket *socket = static_cast<TcpSocket*>(parser->data);
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
    static_cast<TcpSocket*>(parser->data)->appendData(p, static_cast<unsigned int>(len));
    return 0;
}

int onMessageComplete(http_parser *)
{
    return 0;
}

TcpSocket::TcpSocket(const QString &consolePath, const QString &adminPassHash, const QSharedPointer<PathTree> &pathTree)
    :QTcpSocket(),
      m_isNew(true),
      m_request(),
      m_response(),
      m_suicideTimer(this),
      m_uuid(QUuid::createUuid()),
      m_consolePath(consolePath),
      m_adminPassHash(adminPassHash),
      m_pathTree(pathTree)
{
     connect(&m_suicideTimer, SIGNAL(timeout()), this, SLOT(disconnectSocket()));
     connect(this, SIGNAL(shutdown()),this,SLOT(disconnectSocket()));
}

void TcpSocket::handleConsole(HttpRequest &request, HttpResponse &response)
{
    if (request.getHeader().getHeaderInfo().contains("swiftly-admin"))
    {
        QString pass = *request.getHeader().getHeaderInfo()["swiftly-admin"];

        QByteArray hash = QCryptographicHash::hash(pass.toUtf8(), QCryptographicHash::Sha512);

        if (hash.toHex() == m_adminPassHash)
        {
            if (request.getHeader().hasQueries() && request.getHeader().getQueries().contains("cmd"))
            {
                QString cmd = *request.getHeader().getQueries()["cmd"];

                if (cmd == "shutdown")
                {
                    emit shutdown();
                }

                response.setStatusCode(200);
                response << "done!";
                response.finish();
            }
            else
            {
                response.setStatusCode(200);
                response << tem.c_str();
                response.finish();
            }

            return;
        }
    }

    response.setStatusCode(404);
    response << "authentication failed";
    response.finish();
}


bool TcpSocket::setSocketDescriptor(qintptr socketDescriptor, SocketState state,
                         OpenMode openMode)
{
    bool ret = QTcpSocket::setSocketDescriptor(socketDescriptor, state, openMode);
    m_request.reset(new HttpRequest(peerAddress()));
    m_response.reset(new HttpResponse);
    connect(this, SIGNAL(readyRead()), this, SLOT(readClient()));
    connect(this, SIGNAL(disconnected()), this, SLOT(discardClient()));
    setTimeout(1000*60*2);
    return ret;
}

void TcpSocket::discardClient()
{
#ifndef NO_LOG
    qDebug() << "thread id" << thread()->currentThreadId();
    qDebug() << "release socket: " << m_uuid;
    //qDebug() << "finish serving client inside" << m_name;
    sLog() << "finished request.";
    sLogFlush();
#endif
    emit deleteSocket(m_uuid);
}

void TcpSocket::readClient()
{
    //if (disabled)
    //    return;

    // This slot is called when the client sent data to the server. The
    // server looks if it was a get request and sends a very simple HTML
    // document back.
    if(bytesAvailable())
    {
        if ( isNewSocket())
        {
            QByteArray incomingContent=readAll();

            http_parser_settings settings;

            settings. on_message_begin=onMessageBegin;
            settings. on_url=onUrl;
            settings. on_header_field=onHeaderField;
            settings. on_header_value=onHeaderValue;
            settings. on_headers_complete=onHeadersComplete;
            settings. on_body=onBody;
            settings. on_message_complete=onMessageComplete;

            http_parser_init(&m_parser,HTTP_REQUEST);

            m_parser.data = this;

            size_t nparsed = http_parser_execute(&m_parser,&settings,incomingContent.constData(),incomingContent.count());

            if(m_parser.upgrade)
            {
                qDebug()<<"upgrade";
            }
            else if(nparsed != static_cast<size_t>(incomingContent.count()))
            {
                qDebug()<<"nparsed:"<<nparsed<<"buffer size:"<<incomingContent.count();
            }
            else
            {
                //qDebug()<<"parsing seems to be succeed!";
            }

            setRawHeader(incomingContent);

            bool isBodySizeOK=false;
            unsigned int bodySize = 0;
            QSharedPointer<QString> contentLength = getHeader().getHeaderInfo("Content-Length");

            if (!contentLength.isNull())
            {
                bodySize = contentLength->toUInt(&isBodySizeOK);

                if(isBodySizeOK==false)
                {
                    bodySize=0;
                }
            }

            setTotalBytes(bodySize);

            notNew();
        }
        else
        {
            qDebug()<<"socket size:"<<getTotalBytes()<<"current Size:"<<getBytesHaveRead();
            QByteArray incomingContent=readAll();
            appendData(incomingContent);
        }

        if (getBytesHaveRead() > (16*1024*1024))
        {
            getResponse().setStatusCode(400);
            getResponse() << "maximum message size above 16mb.";
            getResponse().finish();
            disconnectFromHost();
            return;
        }
        else
        if(isEof())
        {
            PathTreeNode::HttpVerb handlerType;

            if(m_parser.method==HTTP_GET)
            {
                getHeader().setHttpMethod(HttpHeader::HttpMethod::HTTP_GET);
                handlerType=PathTreeNode::GET;
            }
            else if(m_parser.method==HTTP_POST)
            {
                getHeader().setHttpMethod(HttpHeader::HttpMethod::HTTP_POST);
                handlerType=PathTreeNode::POST;
            }
            else
            {
                qDebug()<<"not get and post"<<atEnd()<<bytesAvailable()<<ConnectedState;
                disconnectFromHost();
                return;
            }

            getRequest().processCookies();
            getRequest().parseFormData();

            //qDebug() << "path" << socket->getRequest().getHeader().getPath();
#ifndef NO_LOG
            sLog() << "handle request:" << getRequest().getHeader().getPath();
            qDebug() << "handle request:" << getRequest().getHeader().getPath();
#endif
            if (!m_consolePath.isEmpty() && m_consolePath == getRequest().getHeader().getPath())
            {
                handleConsole(getRequest(), getResponse());
                getResponse().finish();
            }
            else
            {

                const std::function<void (HttpRequest &, HttpResponse &)> &th = m_pathTree->getTaskHandlerByPath(getRequest().getHeader().getPath(), handlerType);

                if(th)
                {
                    th(getRequest(), getResponse());
                    getResponse().finish();

                }
                else
                {
#ifndef NO_LOG
                    qDebug()<<"empty task handler!" << getRequest().getHeader().getPath() << ";" <<handlerType;
                    sLog()<<"empty task handler!" << getRequest().getHeader().getPath() << ";" <<handlerType;
#endif
                    getResponse().setStatusCode(404);
                    getResponse().finish();
                }
            }
            write( getResponse().m_headerString.toUtf8());
            write(getResponse().m_buffer);

            //emit shutdown();
        }
        else
        {
            qDebug()<<"socket size:"<<getTotalBytes()<<"current size:"<<getBytesHaveRead();
            return;
        }
    }
}

void TcpSocket::setTimeout(int msec)
{
    m_suicideTimer.setSingleShot(true);
    m_suicideTimer.start(msec);
}

void TcpSocket::disconnectSocket()
{
    disconnectFromHost();
}

TcpSocket::~TcpSocket()
{
    if (m_suicideTimer.isActive())
    {
        m_suicideTimer.stop();
    }
}

void TcpSocket::appendData(const char* buffer,unsigned int size)
{
    m_request->appendData(buffer,size);
}

void TcpSocket::appendData(const QByteArray &buffer)
{
    m_request->appendData(buffer);
}

void TcpSocket::setRawHeader(const QString &in)
{
    m_request->setRawHeader(in);
}

QString & TcpSocket::getRawHeader()
{
    return m_request->getRawHeader();
}

unsigned int TcpSocket::getTotalBytes()
{
    return m_request->getTotalBytes();
}

unsigned int TcpSocket::getBytesHaveRead()
{
    return m_request->getBytesHaveRead();
}

HttpHeader & TcpSocket::getHeader()
{
    return m_request->getHeader();
}

bool TcpSocket::isEof()
{
    return (m_isNew == false) && (m_request->getTotalBytes() <= m_request->getBytesHaveRead());
}

void TcpSocket::notNew()
{
    m_isNew = false;
}

bool TcpSocket::isNewSocket()
{
    return m_isNew;
}

void TcpSocket::setTotalBytes(unsigned int totalBytes)
{
    m_request->setTotalBytes(totalBytes);
}


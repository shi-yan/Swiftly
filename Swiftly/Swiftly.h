#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#define REGISTER_WEBAPP( webapp ) \
    do { \
      HttpServer::getSingleton().registerWebApp<webapp>(#webapp);\
    } while (0)

#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QtCore/QStringList>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include "Worker.h"

//class IncomingConnectionQueue;

class HttpServer : public QTcpServer
 {
     Q_OBJECT

    int m_connectionCount;
    bool m_disabled;
    //IncomingConnectionQueue *m_incomingConnectionQueue;

    QVector<int> m_webAppSet;
    int m_count;
    QVector<Worker*> m_workerPool;

    void incomingConnection(qintptr handle) override;
public:
    HttpServer(QObject* parent = nullptr);
    virtual ~HttpServer() override;

    static HttpServer &getSingleton()
    {
        static HttpServer obj;
        return obj;
    }

    void start(int numOfWorkers, quint16 port);
    void pause();
    void resume();

    template<typename T>
    void registerWebApp(const char * webAppName)
    {
        int id = qRegisterMetaType<T>(webAppName);
        m_webAppSet.push_back(id);
    }

public slots:
    void shutdown();
 };

void urlParameterParser(const QByteArray &parameters, QHash<QString, QString> &parameterList);
void generateHashCode(QByteArray &activationCode);
void generateRedirectPageContent(QString &content, const QString &url, unsigned int delay = 1);

#endif // HTTPSERVER_H

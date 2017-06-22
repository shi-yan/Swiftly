#ifndef INCOMINGCONNECTIONQUEUE_H
#define INCOMINGCONNECTIONQUEUE_H

#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <QDebug>
#include <QtCore/QCoreApplication>
#include <QSemaphore>

class IncomingConnectionQueue : public QObject
{
    Q_OBJECT

    QQueue<qintptr> m_socketQueue;
    QMutex m_accessMutex;
    QSemaphore m_fullSemaphore;

public:
    IncomingConnectionQueue(QObject *parent);
    ~IncomingConnectionQueue();

    qintptr getSocket();

    void addSocket(qintptr fd);
};

#endif // INCOMINGCONNECTIONQUEUE_H

#ifndef INCOMMINGCONNECTIONQUEUE_H
#define INCOMMINGCONNECTIONQUEUE_H

#include <QObject>
#include <QQueue>
#include <QMutex>
#include <QWaitCondition>
#include <QDebug>
#include <QtCore/QCoreApplication>

class InCommingConnectionQueue:public QObject
{
    Q_OBJECT

    QQueue<int> socketQueue;

    bool newTaskComming;

    QMutex accessMutex;
    QWaitCondition hasNewSocket;

    InCommingConnectionQueue();
    ~InCommingConnectionQueue();

public:
    static InCommingConnectionQueue & getSingleton()
    {
        static InCommingConnectionQueue obj;
        return obj;
    }

    int getATask()
    {
        int socketDescriptor=-1;
        accessMutex.lock();
      //  qDebug()<<"wait"<<newTaskComming<<socketQueue.size();
        if(newTaskComming || socketQueue.size()==0)
        {

            accessMutex.unlock();

            return -1;

        }

        if(socketQueue.size()>0)
        {
            socketDescriptor=socketQueue.dequeue();
        }
        qDebug()<<"task dequeued!";
        accessMutex.unlock();

        return socketDescriptor;
    }

    void insertATask(int socket)
    {
        newTaskComming=true;
        accessMutex.lock();

            socketQueue.enqueue(socket);



         //
            newTaskComming=false;
            qDebug()<<"task enqueued!";

        accessMutex.unlock();
    }


};

#endif // INCOMMINGCONNECTIONQUEUE_H

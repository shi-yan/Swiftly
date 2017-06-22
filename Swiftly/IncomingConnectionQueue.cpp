#include "IncomingConnectionQueue.h"

IncomingConnectionQueue::IncomingConnectionQueue(QObject *parent)
    :QObject(parent),
      m_socketQueue(),
      m_accessMutex()
{   
}

IncomingConnectionQueue::~IncomingConnectionQueue()
{
}

qintptr IncomingConnectionQueue::getSocket()
{
    qintptr socketDescriptor = -1;
    m_fullSemaphore.acquire();
    m_accessMutex.lock();
    if(m_socketQueue.size()>0)
    {
        socketDescriptor = m_socketQueue.dequeue();
    }
    m_accessMutex.unlock();

    return socketDescriptor;
}

void IncomingConnectionQueue::addSocket(qintptr fd)
{
    m_accessMutex.lock();
    m_socketQueue.enqueue(fd);
    m_accessMutex.unlock();
    m_fullSemaphore.release();
}

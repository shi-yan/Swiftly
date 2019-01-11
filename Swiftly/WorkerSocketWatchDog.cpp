#include "WorkerSocketWatchDog.h"

WorkerSocketWatchDog::WorkerSocketWatchDog(Worker *owner)
    : QThread(owner),
      m_owner(owner),
      m_isStopRequested(false)
{
    connect(this, SIGNAL(newSocketReceived(qintptr)),owner,SLOT(newSocket(qintptr)));
}

WorkerSocketWatchDog::~WorkerSocketWatchDog()
{
}

void WorkerSocketWatchDog::run()
{
    while(!m_isStopRequested)
    {
        qintptr socketFd = m_owner->getSocket();

        if (socketFd == -1)
        {
            m_isStopRequested = true;
        }
        else
        {
            emit newSocketReceived(socketFd);
            m_owner->waitForIdle();
        }
    }
}

void WorkerSocketWatchDog::stop()
{
    m_isStopRequested = true;
}

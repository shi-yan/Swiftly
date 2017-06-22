#ifndef WORKERSOCKETWATCHDOG_H
#define WORKERSOCKETWATCHDOG_H

#include <QObject>
#include <QSemaphore>
#include "IncomingConnectionQueue.h"
#include "Worker.h"

class WorkerSocketWatchDog : public QThread
{
    Q_OBJECT
private:
    Worker *m_owner;
    bool m_isStopRequested;

public:
    WorkerSocketWatchDog(Worker *m_owner);
    ~WorkerSocketWatchDog();

    void run() override;
    void stop();

signals:
    void newSocketReceived(qintptr fd);
};

#endif // WORKERSOCKETWATCHDOG_H

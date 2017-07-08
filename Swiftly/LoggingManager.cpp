#include "LoggingManager.h"
#include <QDebug>

QMutex LoggingManager::m_initMutex;

LoggingManager::LoggingManager()
    :m_logFoldername(QCoreApplication::applicationName().replace(' ','_') % QDateTime::currentDateTime().toString("/yyyy/MM/dd/") %
                     QString::number( QCoreApplication::applicationPid()))
{
    qDebug() << "Log file location:" << m_logFoldername;
    m_logDir = QDir::home();
    if (m_logDir.mkpath(".SwiftlyLogs/" % m_logFoldername))
    {
        m_logDir.cd(".SwiftlyLogs/" % m_logFoldername);
    }
    else
    {
        qDebug() << "can't create log folder";
        m_logDir = QDir::temp();
    }
}

LogEndpoint::LogEndpoint(const QString &filename)
    :m_logFile(filename),
      m_logStream(&m_logFile)
{
    m_logFile.open(QFile::Append);
}

LogEndpoint::~LogEndpoint()
{
    m_logStream.flush();
    m_logFile.close();
}

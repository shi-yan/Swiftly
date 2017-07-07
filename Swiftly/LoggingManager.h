#ifndef LOGGINGMANAGER_H
#define LOGGINGMANAGER_H

#include <QTextStream>
#include <QThreadStorage>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QStringBuilder>
#include <QDateTime>
#include <QThread>
#include <QStandardPaths>

class LoggingManager
{
    QThreadStorage<QTextStream*> m_logStream;
    QThreadStorage<QFile*> m_logFile;
    LoggingManager();

public:
    static LoggingManager & getSingleton()
    {
        static LoggingManager obj;
        return obj;
    }


    QTextStream & log()
    {
        if (m_logStream.hasLocalData())
        {
            return *m_logStream.localData();
        }
        else
        {
            QString logFilename = "SwiftlyLog_" % QDateTime::currentDateTime().toString("yyyy_MM_dd_HH_mm_ss_zzz") %
                    "_" % QString::number( QCoreApplication::applicationPid()) % "_" % QString::number(reinterpret_cast<quint64>(QThread::currentThreadId()), 16) % ".log";

            QFile *logFile = new QFile(QDir::homePath() % "/" % logFilename);
            m_logFile.setLocalData(logFile);
            logFile->open(QFile::Append);
            QTextStream *ts = new QTextStream(logFile);

            m_logStream.setLocalData(ts);
            return *m_logStream.localData();
        }
    }
};


#endif // LOGGINGMANAGER_H

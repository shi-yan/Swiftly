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
#include <QDateTime>
#include <QMutex>

class LogEndpoint
{
    QFile m_logFile;
    QTextStream m_logStream;

    inline LogEndpoint &space() { m_logStream << ' '; return *this; }


public:
    enum class LogLevel
    {
        INFO,
        DEBUG,
        WARNING,
        ERROR
    };

    LogEndpoint(const QString &filename);
    ~LogEndpoint();

    inline LogEndpoint &header(LogLevel level = LogLevel::INFO, bool newLine = true)
    {
        if (newLine)
        {
            m_logStream.flush();
            m_logStream << "\n";
        }

        m_logStream << QDateTime::currentDateTime().toString("[yyyy/MM/dd HH:mm:ss:zzz] ");

        switch (level)
        {
        case LogLevel::DEBUG:
            m_logStream << "[DEBUG] ";
            break;
        case LogLevel::WARNING:
            m_logStream << "[WARNING] ";
            break;
        case LogLevel::ERROR:
            m_logStream << "[ERROR] ";
            break;
        default:
        case LogLevel::INFO:
            m_logStream << "[INFO] ";
            break;
        }
        return *this;
    }

    inline LogEndpoint &operator<<(bool t) { m_logStream << (t ? "true" : "false"); return space(); }
    inline LogEndpoint &operator<<(char t) { m_logStream << t; return space(); }
    inline LogEndpoint &operator<<(signed short t) { m_logStream << t; return space(); }
    inline LogEndpoint &operator<<(unsigned short t) { m_logStream << t; return space(); }
    inline LogEndpoint &operator<<(signed int t) { m_logStream << t; return space(); }
    inline LogEndpoint &operator<<(unsigned int t) { m_logStream << t; return space(); }
    inline LogEndpoint &operator<<(signed long t) { m_logStream << t; return space(); }
    inline LogEndpoint &operator<<(unsigned long t) {m_logStream << t; return space(); }
    inline LogEndpoint &operator<<(qint64 t) { m_logStream << t; return space(); }
    inline LogEndpoint &operator<<(quint64 t) { m_logStream << t; return space(); }
    inline LogEndpoint &operator<<(float t) { m_logStream << t; return space(); }
    inline LogEndpoint &operator<<(double t) { m_logStream << t; return space(); }
    inline LogEndpoint &operator<<(const char* t) { m_logStream << QString::fromUtf8(t); return space(); }
    inline LogEndpoint &operator<<(const QString & t) { m_logStream << t; return space(); }
    inline LogEndpoint &operator<<(const QStringRef & t) { m_logStream << t; return space(); }
    inline LogEndpoint &operator<<(QLatin1String t) { m_logStream << t; return space(); }
    inline LogEndpoint &operator<<(const QByteArray & t) { m_logStream << t; return space(); }
    inline LogEndpoint &operator<<(const void * t) { m_logStream << t; return space(); }
    inline LogEndpoint &operator<<(std::nullptr_t) { m_logStream << "(nullptr)"; return space(); }

    inline void flush() {m_logStream.flush();}

};

class LoggingManager
{
    QThreadStorage<LogEndpoint*> m_logEndpoint;
    QString m_logFoldername;
    QDir m_logDir;
    LoggingManager();
    static QMutex m_initMutex;

public:
    static LoggingManager & getSingleton()
    {
        LoggingManager::m_initMutex.lock();
        static LoggingManager obj;
        LoggingManager::m_initMutex.unlock();
        return obj;
    }

    inline LogEndpoint & log(LogEndpoint::LogLevel level = LogEndpoint::LogLevel::INFO)
    {
        if (m_logEndpoint.hasLocalData())
        {
            return (*m_logEndpoint.localData()).header(level);
        }
        else
        {
            QString logFilename = QCoreApplication::applicationName().replace(' ','_') % "_" %
                    QString::number( QCoreApplication::applicationPid()) % "_"
                    % QString::number(reinterpret_cast<quint64>(QThread::currentThreadId()), 16) % ".log";
            LogEndpoint *endpoint = new LogEndpoint(m_logDir.absoluteFilePath(logFilename));
            m_logEndpoint.setLocalData(endpoint);
            return (*m_logEndpoint.localData()).header(level, false);
        }
    }

    inline void flush()
    {
        if (m_logEndpoint.hasLocalData())
        {
            m_logEndpoint.localData()->flush();
        }
    }
};

inline LogEndpoint & sLog(LogEndpoint::LogLevel level = LogEndpoint::LogLevel::INFO)
{
    return LoggingManager::getSingleton().log(level);
}

inline void sLogFlush()
{
    LoggingManager::getSingleton().flush();
}


#endif // LOGGINGMANAGER_H

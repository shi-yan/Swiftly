#ifndef SESSIONMANAGER_H
#define SESSIONMANAGER_H

#include <QByteArray>

class SessionManager
{
public:
    SessionManager();

    bool newSession(const QString &userId, const QString &email, QByteArray &sessionId);
    bool findSession(const QByteArray &sessionId, QString &email);
    bool logoutSession(const QByteArray &sessionId);

    static void init();

private:
    void generateSessionId(QByteArray &sessionId);
};

#endif // SESSIONMANAGER_H

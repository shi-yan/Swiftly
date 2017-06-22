#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QString>
#include <QObject>

class UserManager : public QObject
{
    Q_OBJECT
public:
    UserManager();

    bool signup(const QString &email, const QString &password);
    bool login(const QString &email, const QString &password);
    bool resetPassword(const QString &email, const QString &newPassword, const QString &resetCodeOrOldPassword, bool useOldPassword);
    bool activate(const QString &email, const QString &activationCode);
    bool sendResetRequest(const QString &email);
    bool sendActivationCode(const QString &email);

    static bool isValidEmail(const QString &email);
    static bool isStrongPassword(const QString &password);
};

#endif // USERMANAGER_H

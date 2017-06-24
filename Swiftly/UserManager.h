#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QString>
#include <QObject>

class UserManager : public QObject
{
    Q_OBJECT
public:
    UserManager();

    bool signup(const QString &email, const QByteArray &password, const QMap<QString, QVariant> &extraFields);
    bool login(const QString &email, const QByteArray &password, QMap<QString, QVariant> &extraFields);
    bool resetPassword(const QString &email, const QByteArray &newPassword, const QByteArray &resetCodeOrOldPassword, bool useOldPassword);
    bool activate(const QString &email, const QString &activationCode);
    bool sendResetRequest(const QString &email);
    bool sendActivationCode(const QString &userId, const QString &email);
    bool updateEmail(const QString &userId, const QString &newEmail);

    static bool isValidEmail(const QString &email);
    static bool isValidePassword(const QByteArray &password, QString &errorMessage, unsigned int upperCaseNum = 1, unsigned int specialLetterNum = 1,
                                 unsigned int digitNum = 1, unsigned int lowerCaseLetter = 1, unsigned int length = 8, unsigned int maxLength = 30);
    static bool verifyPassword(const QByteArray &hash, const QByteArray &password);
    static void hashPassword(const QByteArray &password, QByteArray &hash);

    static void generateActivationCode(const QString &email, QByteArray &activationCode);

};

#endif // USERMANAGER_H

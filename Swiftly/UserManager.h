#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QString>
#include <QObject>

class UserManager : public QObject
{
    Q_OBJECT
public:
    UserManager();

    UserManager(const UserManager &)
        :QObject()
    {

    }

    void operator=(const UserManager &)
    {

    }

    bool signup(const QString &email, const QByteArray &password, const QMap<QString, QVariant> &extraFields, QString &errorMessage, QByteArray &activationCode);
    bool login(const QString &email, const QByteArray &password, QString &userId, QMap<QString, QVariant> &extraFields, QString &errorMessage);
    bool resetPassword(const QString &email, const QByteArray &newPassword, const QByteArray &resetCodeOrOldPassword, bool useOldPassword);
    bool activate(QString &email, const QByteArray &activationCode);
    bool generatePasswordResetRequest(const QString &email, QByteArray &resetCode);
    bool getActivationCode(const QString &email, QByteArray &activationCode);
    bool getUser(const QString &email, QMap<QString, QVariant> &fields);
    bool updateEmail(const QString &userId, const QString &newEmail);

    static bool isValidEmail(const QString &email);
    static bool isValidePassword(const QByteArray &password, QString &errorMessage, unsigned int upperCaseNum = 1, unsigned int specialLetterNum = 1,
                                 unsigned int digitNum = 1, unsigned int lowerCaseLetter = 1, unsigned int length = 8, unsigned int maxLength = 30);
    static bool verifyPassword(const QByteArray &hash, const QByteArray &password);
    static void hashPassword(const QByteArray &password, QByteArray &hash);

    static void generateActivationCode(const QString &email, QByteArray &activationCode);
    static void generatePasswordResetCode(const QString &email, QByteArray &resetCode);

};

#endif // USERMANAGER_H

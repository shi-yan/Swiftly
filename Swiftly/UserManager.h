#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QString>
#include <QObject>

//! This class handles all user registration and login logic
class UserManager : public QObject
{
    Q_OBJECT
public:
    //! Constructor
    //! both constructors do nothing, the class members need
    //! to be pure functions.
    UserManager();

    UserManager(const UserManager &)
        :QObject()
    {
    }

    void operator=(const UserManager &)
    {

    }

    bool signup(const QString &email, const QByteArray &password,
                const QMap<QString, QVariant> &extraFields, QString &errorMessage, QByteArray &activationCode);

    bool login(const QString &email, const QByteArray &password, QString &userId, QMap<QString, QVariant> &extraFields, QString &errorMessage);

    bool resetPassword(const QString &email, const QByteArray &newPassword, const QByteArray &resetCodeOrOldPassword,
                       bool useOldPassword, QString &errorMessage);
    bool activate(QString &email, const QByteArray &activationCode, QString &errorMessage);

    bool generatePasswordResetRequest(const QString &email, QByteArray &resetCode, QString &errorMessage);
    bool getActivationCode(const QString &email, QByteArray &activationCode, QString &errorMessage);
    bool getUser(const QString &email, QMap<QString, QVariant> &fields);

    static bool isValidEmail(const QString &email);
    static bool isValidePassword(const QByteArray &password, QString &errorMessage, unsigned int upperCaseNum = 1, unsigned int specialLetterNum = 1,
                                 unsigned int digitNum = 1, unsigned int lowerCaseLetter = 1, unsigned int length = 8, unsigned int maxLength = 30);
    static bool verifyPassword(const QByteArray &hash, const QByteArray &password);
    static void hashPassword(const QByteArray &password, QByteArray &hash);

    static void generateActivationCode(const QString &email, QByteArray &activationCode);
    static void generatePasswordResetCode(const QString &email, QByteArray &resetCode);

};

#endif // USERMANAGER_H

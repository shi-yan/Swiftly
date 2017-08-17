#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QString>
#include <QObject>

/*! \brief UserManager handles all user registration and login logic
 *
 *  Everything related to user signup/signin is implemented in this class.
 */
class UserManager : public QObject
{
    Q_OBJECT
public:
    //! \brief Constructor is empty
    /*! both constructors do nothing, the class members need
    * to be pure functions.
    */
    UserManager();

    UserManager(const UserManager &)
        :QObject()
    {
    }

    //! the assignment operator shouldn't do anything for this class
    /*!
     * \param in the input
     * \sa UserManager()
     * Assignment operator shouldn't do anything.
     */
    void operator=(const UserManager &in)
    {
        /*!
        * We don't want to implement assigment operator
        */

    }

    //! \brief signup a user
    /*!
    * \param email unique email address
    * \param password complex password
    * \param extraFields a dictionary of extra meta data that needed to be written into user account
    * \param errorMessage error happened during signup
    * \param activationCode the activation code that will be sent to activate the user account
    * This function signs up a user. The user's email must be unique in the database, the password needs to conform to the complext
    * password standard. The parameter extraFields holds extra meta data that should be written to the user's database entry.
    * Any error messages will be returned via errorMessage. If user is successfully signed up, an activation code is returned
    * via activationCode.
    */
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

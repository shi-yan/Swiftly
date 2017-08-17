#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QString>
#include <QObject>

/*! \brief UserManager handles all user registration and login logic
 *
 *  Everything related to user signup/signin is implemented in this class. This class has no rest api.
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
    * \param[in] email unique email address
    * \param[in] password complex password
    * \param[in] extraFields a dictionary of extra meta data that needed to be written into user account
    * \param[out] errorMessage error happened during signup
    * \param[out] activationCode the activation code that will be sent to activate the user account
    * \return true if user is signed up, false if met with errors
    *
    * This function signs up a user. The user's email must be unique in the database, the password needs to conform to the complext
    * password standard. The parameter extraFields holds extra meta data that should be written to the user's database entry.
    * Any error messages will be returned via errorMessage. If user is successfully signed up, an activation code is returned
    * via activationCode.
    */
    bool signup(const QString &email, const QByteArray &password,
                const QMap<QString, QVariant> &extraFields, QString &errorMessage, QByteArray &activationCode);

    //! \brief login a user
    /*!
    * \param[in] email user's email account
    * \param[in] password user's password
    * \param[out] userId the unique id of this user
    * \param[out] extraFields once logged in, the extra meta data saved to the user's account will be retrieved and returned via extraFields
    * \param[out] errorMessage errors happened during logging in
    * \return true if logged in, false if met with errors
    *
    * This function log a user in, it checks the existence of the user and verifies the user's password. If succeeded, the userId and meta data
    * will be returned.
    */
    bool login(const QString &email, const QByteArray &password, QString &userId, QMap<QString, QVariant> &extraFields, QString &errorMessage);

    /*!
     * \brief reset user's password
     *
     * \param[in] email user's email address
     * \param[in] newPassword the new password
     * \param[in] resetCodeOrPassword a string that is ei the old password, or by using a reset code.
     * \param[in] useOldPassword a boolean indicating if user is resetting using the old password or use a reset code.
     * \param[out] errorMessage error messages
     * \return true if password is reset, false if met with errors
     *
     * There are 2 ways to reset a password. Either the user still remembers the old password and uses that to update the password, or
     * the user can't remember the old passowrd, in which case, he/she needs a reset code sent via email.
     */
    bool resetPassword(const QString &email, const QByteArray &newPassword, const QByteArray &resetCodeOrOldPassword,
                       bool useOldPassword, QString &errorMessage);

    /*!
     * \brief activate an user by an activation code
     * \param[in] email the user account identity
     * \param[in] activationCode the activation code sent via email
     * \param[out] errorMessage the error met while activating the account
     * \return true if account is activated, false if met with errors
     *
     * Newly signed up account needs to be activated to verify the email address. This function check the provided activation code with
     * pending activation requests.
     */
    bool activate(QString &email, const QByteArray &activationCode, QString &errorMessage);

    /*!
     * \brief generatePasswordResetRequest generate a password reset code for an user
     * \param[in] email the user's identify
     * \param[out] resetCode the reset code that will be sent to the user via email.
     * \param[out] errorMessage any error messages generated during the process.
     * \return true if reset code generated, otherwise false
     *
     * If an user loses password, he/she can request a reset code sent to the user's email to reset the password. This function only
     * generate the reset code and save it into the database. The code needs to be sent separately.
     * \sa generateActivationCode()
     */
    bool generatePasswordResetRequest(const QString &email, QByteArray &resetCode, QString &errorMessage);

    /*!
     * \brief getActivationCode generate an activation code for newly signed up users.
     * \param[in] email the user's identity
     * \param[out] activationCode the activation code generated
     * \param[out] errorMessage error messages if any
     * \return true on success, otherwise false
     *
     * Similar to \fn generatePasswordResetRequest(), this function generates an activation code.
     * The code needs to be sent to user via email.
     * \sa generatePasswordResetRequest()
     */
    bool getActivationCode(const QString &email, QByteArray &activationCode, QString &errorMessage);

    /*!
     * \brief getUser a helper function to retrieve an user from the database
     * \param[in] email the user's identity
     * \param[out] fields extra meta data contained in the user's database
     * \return true on success, false on failure
     *
     * Certain operation needs to get user's info. For example, given a session id, we want to obtain the logged in user's information.
     * This is a helper function for that.
     */
    bool getUser(const QString &email, QMap<QString, QVariant> &fields);

    /*!
     * \brief isValidEmail a helper function to verify email address.
     * \param[in] email the email address to be verified
     * \return true on valid email address, otherwise false
     *
     * Email needs to conform to the standard format.
     * \todo need to rule out throw-way email services, such as mailinator.com
     */
    static bool isValidEmail(const QString &email);

    /*!
     * \brief isValidePassword function checks on the validity of a password.
     * \param[in] password the password to be checked.
     * \param[out] errorMessage if password check failed, this parameter contains detailed reasons.
     * \param[in] upperCaseNum at least how many upper case letters
     * \param[in] specialLetterNum at least how many special letters
     * \param[in] digitNum at least how many digits
     * \param[in] lowerCaseLetter at least how many lower case letters
     * \param[in] length the minimum length for the password
     * \param[in] maxLength the maximum length for the password
     * \return true if password check pass, otherwise false
     *
     * Check on the complexity of password.
     */
    static bool isValidePassword(const QByteArray &password, QString &errorMessage, unsigned int upperCaseNum = 1, unsigned int specialLetterNum = 1,
                                 unsigned int digitNum = 1, unsigned int lowerCaseLetter = 1, unsigned int length = 8, unsigned int maxLength = 30);

    /*!
     * \brief verifyPassword verify if a provided password matches a provided hash code
     * \param[in] hash the hash code
     * \param[in] password the password
     * \return true if the hash matches the password.
     *
     * This function uses libsodium
     */
    static bool verifyPassword(const QByteArray &hash, const QByteArray &password);

    /*!
     * \brief hashPassword a helper function to generate the hash code of a provided function
     * \param[in] password the password
     * \param[out] hash the generated hash code
     *
     * This function uses libsodium
     */
    static void hashPassword(const QByteArray &password, QByteArray &hash);

    /*!
     * \brief generateActivationCode helper function to generate complex activation code
     * \param[in] email user identity
     * \param[out] activationCode generated activation code
     *
     * Use libsodium to generated a hard to guess random string as the activation code.
     * \sa generatePasswordResetCode()
     */
    static void generateActivationCode(const QString &email, QByteArray &activationCode);

    /*!
     * \brief generatePasswordResetCode helper function to generate a password reset code
     * \param[in] email the user account
     * \param[out] resetCode the password reset code generated
     *
     * User libsodium to generate a hard to guess random string as the password reset code.
     * \sa generateActivationCode()
     */
    static void generatePasswordResetCode(const QString &email, QByteArray &resetCode);

};

#endif // USERMANAGER_H

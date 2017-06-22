#include "UserManager.h"
#include <QRegularExpression>
#include <QDebug>

UserManager::UserManager()
{

}

bool UserManager::signup(const QString &email, const QString &password)
{

}

bool UserManager::login(const QString &email, const QString &password)
{

}

bool UserManager::resetPassword(const QString &email, const QString &newPassword, const QString &resetCodeOrOldPassword, bool useOldPassword)
{

}

bool UserManager::activate(const QString &email, const QString &activationCode)
{

}

bool UserManager::sendResetRequest(const QString &email)
{

}

bool UserManager::sendActivationCode(const QString &email)
{

}

bool UserManager::isValidEmail(const QString &email)
{
    static QRegularExpression emailRegexp("^[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}$", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = emailRegexp.match(email);
    return match.hasMatch();
}

bool UserManager::isStrongPassword(const QString &password)
{
    //https://stackoverflow.com/questions/5142103/regex-to-validate-password-strength
    static QRegularExpression strongPasswordRegexp("^(?=.*[A-Z].*[A-Z])(?=.*[!@#$&*])(?=.*[0-9].*[0-9])(?=.*[a-z].*[a-z].*[a-z]).{8}$");
    qDebug() << strongPasswordRegexp.isValid();
    QRegularExpressionMatch match = strongPasswordRegexp.match(password);
    qDebug() << "2 upper case"<<match.captured(1);

    if (match.hasPartialMatch())
    {
       // bool 2UpperCaseLetters =
        qDebug() << "2 upper case"<<match.captured(0);
        //bool 1SpecialCaseLetter = match.captured(1);
        //bool 2Digits = match.captured(2);
        //bool 3LowerCaseLetters = match.captured(3);
        //bool atLeast8 = match.captured(4);

    }
    else
    {
        return false;
    }
}

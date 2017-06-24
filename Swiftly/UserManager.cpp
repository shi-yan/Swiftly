#include "UserManager.h"
#include <QRegularExpression>
#include <QDebug>
#include <sodium.h>
#include <QString>
#include <QStringBuilder>

#include <cstdint>
#include <iostream>
#include <vector>
#include <bsoncxx/json.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;


UserManager::UserManager()
{

}

bool UserManager::signup(const QString &email, const QByteArray &password)
{
    if (!isValidEmail(email))
    {
        return false;
    }

    QString errorMessage;
    if (!isValidePassword(password, errorMessage))
    {
        return false;
    }

    QByteArray hash;
    hashPassword(password, hash);


    mongocxx::instance instance{};
    mongocxx::client client{mongocxx::uri{}};

    mongocxx::database swiftlyDb = client["Swiftly"];
    mongocxx::collection userCollection = swiftlyDb["User"];

    bsoncxx::builder::stream::document index_builder;
    mongocxx::options::index index_options{};
    index_builder << "email" << 1;
    index_options.unique(true);
    userCollection.create_index(index_builder.view(), index_options);

    auto builder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value doc_value = builder
      << "email" << email.toStdString().c_str()
      << "password" << hash.toStdString().c_str()
      << bsoncxx::builder::stream::finalize;

    mongocxx::stdx::optional<mongocxx::result::insert_one> result =
    userCollection.insert_one(doc_value.view());

    if (result)
    {
        bsoncxx::oid oid = (*result).inserted_id().get_oid().value;
        std::string userId = oid.to_string();
        qDebug() << userId.c_str() << hash;
    }
    return true;
}

bool UserManager::login(const QString &email, const QByteArray &password)
{

}

bool UserManager::resetPassword(const QString &email, const QByteArray &newPassword, const QByteArray &resetCodeOrOldPassword, bool useOldPassword)
{

}

bool UserManager::activate(const QString &email, const QString &activationCode)
{

}

bool UserManager::sendResetRequest(const QString &email)
{

}

bool UserManager::sendActivationCode(const QString &userId, const QString &email)
{

}

bool UserManager::isValidEmail(const QString &email)
{
    static QRegularExpression emailRegexp("^[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}$", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = emailRegexp.match(email);
    return match.hasMatch();
}

bool UserManager::isValidePassword(const QByteArray &password, QString &errorMessage, unsigned int upperCaseNum, unsigned int specialLetterNum,
                                   unsigned int digitNum, unsigned int lowerCaseNum, unsigned int length, unsigned int maxLength)
{
    errorMessage = "";

    QString passwordStr = QString::fromLatin1(password);

    if (passwordStr.length() < length)
    {
        errorMessage = "Password needs to be at least " % QString::number(length) % " in length.";
        return false;
    }

    if (passwordStr.length() > maxLength)
    {
        errorMessage = "Password can't be longer than " % QString::number(maxLength) % ".";
        return false;
    }

    static QRegularExpression validCharactersRegexp("^[A-Z0-9._%+-@#!$&*]+$", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = validCharactersRegexp.match(QString::fromLatin1(password));

    if (!match.hasMatch())
    {
        errorMessage = "Only A-Z, a-z, 0-9, ._%+-!@#$&* can be used in password.";
        return false;
    }

    unsigned int upperCase = 0;
    unsigned int specialLetter = 0;
    unsigned int digit = 0;
    unsigned int lowerCase = 0;

    for (int i = 0; i< passwordStr.length(); ++i)
    {
        QChar character = passwordStr.at(i);
        if (character.isDigit())
        {
            digit++;
        }
        else if(character.isLetter())
        {
            if (character.isUpper())
            {
                upperCase++;
            }
            else if(character.isLower())
            {
                lowerCase++;
            }
        }
        else
        {
            specialLetter++;
        }

        if ((upperCase >= upperCaseNum) &&
                (specialLetter >= specialLetterNum) &&
                (digit >= digitNum) &&
                (lowerCase >= lowerCaseNum))
        {
            return true;
        }
    }

    if (upperCase < upperCaseNum)
    {
        errorMessage = "Password needs to contain at least " % QString::number(upperCaseNum) % " upper case letter" % ((upperCaseNum > 1)?"s.":".");
    }
    else if (specialLetter < specialLetterNum)
    {
        errorMessage = "Password needs to contain at least " % QString::number(specialLetterNum) % " special letter" % ((specialLetterNum > 1)?"s.":".");
    }
    else if (digit < digitNum)
    {
        errorMessage = "Password needs to contain at least " % QString::number(digitNum) % " digit" % ((digitNum > 1)?"s.":".");
    }
    else if (lowerCase < lowerCaseNum)
    {
        errorMessage = "Password needs to contain at least " % QString::number(lowerCaseNum) % " lower case letter" % ((lowerCaseNum > 1)?"s.":".");
    }
    return false;
}

bool UserManager::verifyPassword(const QByteArray &hash, const QByteArray &password)
{
    QString errorMessage = "";

    if (crypto_pwhash_str_verify(hash.data(), password.data(),
                                 password.size()) == 0)
    {
        return true;
    }
    return false;
}

void UserManager::hashPassword(const QByteArray &password, QByteArray &hash)
{
    hash.resize(crypto_pwhash_STRBYTES);
    if (crypto_pwhash_str(hash.data(), password.data(), password.size(),
         crypto_pwhash_OPSLIMIT_SENSITIVE, crypto_pwhash_MEMLIMIT_SENSITIVE) != 0)
    {
        qDebug() << "out of memory";
    }
}

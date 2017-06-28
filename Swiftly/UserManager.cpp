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
#include <mongocxx/bulk_write.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <QCryptographicHash>
#include <QDateTime>

using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;


UserManager::UserManager()
{

}

bool UserManager::signup(const QString &email, const QByteArray &password, const QMap<QString, QVariant> &extraFields, QString &errorMessage)
{
    if (!isValidEmail(email))
    {
        errorMessage = "Ill-formed email address";
        return false;
    }

    if (!isValidePassword(password, errorMessage))
    {
        errorMessage = "Ill-formed password:" % errorMessage;
        return false;
    }

    QByteArray hash;
    hashPassword(password, hash);


    mongocxx::client client{mongocxx::uri{}};

    mongocxx::database swiftlyDb = client["Swiftly"];
    mongocxx::collection userCollection = swiftlyDb["User"];
    mongocxx::collection activationCollection = swiftlyDb["Activation"];

    bsoncxx::builder::stream::document userIndexBuilder;
    mongocxx::options::index userIndexOptions{};
    userIndexBuilder << "email" << 1;
    userIndexOptions.unique(true);
    userCollection.create_index(userIndexBuilder.view(), userIndexOptions);

    bsoncxx::builder::stream::document activationEmailIndexBuilder;
    mongocxx::options::index activationEmailOptions{};
    activationEmailIndexBuilder << "email" << 1;
    activationEmailOptions.unique(true);
    activationCollection.create_index(activationEmailIndexBuilder.view(), activationEmailOptions);

    bsoncxx::builder::stream::document activationCodeIndexBuilder;
    mongocxx::options::index activationCodeOptions{};
    activationCodeIndexBuilder << "activation_code" << 1;
    activationCodeOptions.unique(true);
    activationCollection.create_index(activationCodeIndexBuilder.view(), activationCodeOptions);

    auto builder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value userAccountDocumentValue = builder
      << "email" << email.toStdString().c_str()
      << "password" << hash.toStdString().c_str()
      << "status" << 0
      << bsoncxx::builder::stream::finalize;

    try
    {
        mongocxx::stdx::optional<mongocxx::result::insert_one> result =
        userCollection.insert_one(userAccountDocumentValue.view());

        if (result)
        {
            bsoncxx::oid oid = (*result).inserted_id().get_oid().value;
            std::string userId = oid.to_string();

            QByteArray activationCode;
            generateActivationCode(email, activationCode);
            bsoncxx::document::value activationDocumentValue = bsoncxx::builder::stream::document{}
                    << "activation_code" << activationCode.toStdString().c_str()
                    << "time" << static_cast<std::int64_t>( QDateTime::currentDateTimeUtc().toTime_t())
                    << "email" << email.toStdString().c_str()
                    << "userId" << userId.c_str()
                    << bsoncxx::builder::stream::finalize;

            result = activationCollection.insert_one(activationDocumentValue.view());

            if (!result)
            {
                errorMessage = "Unable to generate activation code";
                return false;
            }

            qDebug() << userId.c_str() << hash;
        }
        else
        {
            errorMessage = "Unknown issue when generate user account";
            return false;
        }
    }
    catch(const mongocxx::bulk_write_exception &e)
    {
        if (e.code().value() == 11000)
        {
            //qDebug() << "mongo error: duplicated email:" << e.what();
            errorMessage = "User with the same email exists" % QString::fromLatin1(e.what());
        }
        else
        {
            //qDebug() << "unknown mongo error:" << e.code().value() << e.what();
            errorMessage = "Database error:" % QString::fromLatin1(e.what());
        }
        return false;
    }

    return true;
}

bool UserManager::login(const QString &email, const QByteArray &password, QMap<QString, QVariant> &extraFields)
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


    mongocxx::client client{mongocxx::uri{}};

    mongocxx::database swiftlyDb = client["Swiftly"];
    mongocxx::collection userCollection = swiftlyDb["User"];

    mongocxx::stdx::optional<bsoncxx::document::value> maybe_result =
           userCollection.find_one(bsoncxx::builder::stream::document{}
                                   << "email" << "billconan@gmail.com" << bsoncxx::builder::stream::finalize);

    if(maybe_result)
    {
        //std::cout << bsoncxx::to_json(*maybe_result) << "\n";
        //bsoncxx::document::element emailElement = (*maybe_result).view()["email"];
        //email =  QString::fromStdString(emailElement.get_utf8().value.to_string());
        bsoncxx::document::element passwordElement = (*maybe_result).view()["password"];

        QByteArray passwordHash;
        passwordHash.resize(crypto_pwhash_STRBYTES);
        passwordHash.clear();
        std::string passwordHashStr = passwordElement.get_utf8().value.to_string();
        passwordHash.setRawData(passwordHashStr.data(), passwordHashStr.size());

        qDebug() << passwordHash;

        if (!UserManager::verifyPassword(passwordHash, password))
        {
            return false;
        }

        bsoncxx::document::element statusElement = (*maybe_result).view()["status"];

        std::int64_t status = statusElement.get_int64().value;
        if (status == 0)
        {
            return false;
        }
    }
    else
    {
        return false;
    }


    return true;
}

bool UserManager::resetPassword(const QString &email, const QByteArray &newPassword, const QByteArray &resetCodeOrOldPassword, bool useOldPassword)
{
    if (!isValidEmail(email))
    {
        return false;
    }

    if (!isValidEmail(newPassword))
    {
        return false;
    }


    mongocxx::client client{mongocxx::uri{}};

    mongocxx::database swiftlyDb = client["Swiftly"];
    mongocxx::collection resetCollection = swiftlyDb["Reset"];
    mongocxx::collection userCollection = swiftlyDb["User"];

    QString errorMessage;
    if (useOldPassword)
    {
        if (!isValidePassword(resetCodeOrOldPassword, errorMessage))
        {
            return false;
        }

        mongocxx::stdx::optional<bsoncxx::document::value> maybe_result =
               userCollection.find_one(bsoncxx::builder::stream::document{}
                                       << "email" << "billconan@gmail.com" << bsoncxx::builder::stream::finalize);

        if(maybe_result)
        {
            //std::cout << bsoncxx::to_json(*maybe_result) << "\n";
            //bsoncxx::document::element emailElement = (*maybe_result).view()["email"];
            //email =  QString::fromStdString(emailElement.get_utf8().value.to_string());
            bsoncxx::document::element passwordElement = (*maybe_result).view()["password"];

            QByteArray passwordHash;
            passwordHash.resize(crypto_pwhash_STRBYTES);
            passwordHash.clear();
            std::string passwordHashStr = passwordElement.get_utf8().value.to_string();
            passwordHash.setRawData(passwordHashStr.data(), passwordHashStr.size());

            qDebug() << passwordHash;

            if (!UserManager::verifyPassword(passwordHash, resetCodeOrOldPassword))
            {
                return false;
            }
        }
        else
        {
            return false;
        }

    }
    else
    {

        mongocxx::stdx::optional<bsoncxx::document::value> maybe_result =
               resetCollection.find_one(bsoncxx::builder::stream::document{}
                                       << "reset_code" << resetCodeOrOldPassword.toStdString().c_str()
                                       << bsoncxx::builder::stream::finalize);

        if(maybe_result)
        {
            //std::cout << bsoncxx::to_json(*maybe_result) << "\n";
            //bsoncxx::document::element emailElement = (*maybe_result).view()["email"];
            //email =  QString::fromStdString(emailElement.get_utf8().value.to_string());
            bsoncxx::document::element emailElement = (*maybe_result).view()["email"];

            if (email != QString::fromStdString(emailElement.get_utf8().value.to_string()))
            {
                return false;
            }
        }
        else
        {
            return false;
        }

        if (!isValidePassword(newPassword, errorMessage))
        {
            return false;
        }
    }


    QByteArray hash;
    hashPassword(newPassword, hash);

    try
    {
        //std::cout << bsoncxx::to_json(*maybe_result) << "\n";
        //bsoncxx::document::element emailElement = (*maybe_result).view()["email"];
        //email =  QString::fromStdString(emailElement.get_utf8().value.to_string());

        mongocxx::stdx::optional<mongocxx::result::update> result = userCollection.update_one(bsoncxx::builder::stream::document{} << "email" << email.toStdString().c_str() << finalize,
                                  bsoncxx::builder::stream::document{} << "$set"
                                  << bsoncxx::builder::stream::open_document
                                  << "password" << hash.toStdString().c_str()
                                  << bsoncxx::builder::stream::close_document
                                  << bsoncxx::builder::stream::finalize);


        if (result)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    catch(const mongocxx::bulk_write_exception &e)
    {
        if (e.code().value() == 11000)
        {
            qDebug() << "mongo error: duplicated email:" << e.what();
        }
        else
        {
            qDebug() << "unknown mongo error:" << e.code().value() << e.what();
        }
        return false;
    }

    return true;
}

bool UserManager::activate(QString &email, const QString &activationCode)
{
    mongocxx::client client{mongocxx::uri{}};

    mongocxx::database swiftlyDb = client["Swiftly"];
    mongocxx::collection activationCollection = swiftlyDb["Activation"];
    mongocxx::collection userCollection = swiftlyDb["User"];

    mongocxx::stdx::optional<bsoncxx::document::value> maybe_result =
           activationCollection.find_one(bsoncxx::builder::stream::document{}
                                   << "activation_code" << activationCode.toStdString().c_str()
                                   << bsoncxx::builder::stream::finalize);

    if(maybe_result)
    {
        //std::cout << bsoncxx::to_json(*maybe_result) << "\n";
        //bsoncxx::document::element emailElement = (*maybe_result).view()["email"];
        //email =  QString::fromStdString(emailElement.get_utf8().value.to_string());
        bsoncxx::document::element emailElement = (*maybe_result).view()["email"];

        email = QString::fromStdString(emailElement.get_utf8().value.to_string());


        mongocxx::stdx::optional<mongocxx::result::update> result = userCollection.update_one(bsoncxx::builder::stream::document{} << "email" << email.toStdString().c_str() << finalize,
                                  bsoncxx::builder::stream::document{} << "$set"
                                  << bsoncxx::builder::stream::open_document
                                  << "status" << 1 << bsoncxx::builder::stream::close_document
                                  << bsoncxx::builder::stream::finalize);
        if (result)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}

bool UserManager::sendPasswordResetRequest(const QString &email)
{
    mongocxx::client client{mongocxx::uri{}};

    mongocxx::database swiftlyDb = client["Swiftly"];
    mongocxx::collection resetCollection = swiftlyDb["Reset"];

    bsoncxx::builder::stream::document resetPasswordEmailIndexBuilder;
    mongocxx::options::index resetPasswordEmailOptions{};
    resetPasswordEmailIndexBuilder << "email" << 1;
    resetPasswordEmailOptions.unique(true);
    resetCollection.create_index(resetPasswordEmailIndexBuilder.view(), resetPasswordEmailOptions);

    bsoncxx::builder::stream::document resetPasswordCodeIndexBuilder;
    mongocxx::options::index resetPasswordCodeOptions{};
    resetPasswordCodeIndexBuilder << "reset_code" << 1;
    resetPasswordCodeOptions.unique(true);
    resetCollection.create_index(resetPasswordCodeIndexBuilder.view(), resetPasswordCodeOptions);

    QByteArray passwordResetCode;
    generateActivationCode(email, passwordResetCode);
    bsoncxx::document::value passwordResetDocumentValue = bsoncxx::builder::stream::document{}
            << "reset_code" << passwordResetCode.toStdString().c_str()
            << "time" << static_cast<std::int64_t>( QDateTime::currentDateTimeUtc().toTime_t())
            << "email" << email.toStdString().c_str()
           // << "userId" << userId.c_str()
            << bsoncxx::builder::stream::finalize;

    try
    {
        mongocxx::stdx::optional<mongocxx::result::insert_one> result =
        resetCollection.insert_one(passwordResetDocumentValue.view());

        if (result)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    catch(const mongocxx::bulk_write_exception &e)
    {
        if (e.code().value() == 11000)
        {
            qDebug() << "mongo error: duplicated email:" << e.what();
        }
        else
        {
            qDebug() << "unknown mongo error:" << e.code().value() << e.what();
        }
        return false;
    }

    return true;
}

bool UserManager::sendActivationCode(const QString &email, QString &activationCode)
{
    if (!isValidEmail(email))
    {
        return false;
    }

    mongocxx::client client{mongocxx::uri{}};

    mongocxx::database swiftlyDb = client["Swiftly"];
    mongocxx::collection activationCollection = swiftlyDb["Activation"];
    mongocxx::collection userCollection = swiftlyDb["User"];

    mongocxx::stdx::optional<bsoncxx::document::value> maybe_result =
           activationCollection.find_one(bsoncxx::builder::stream::document{}
                                   << "email" << email.toStdString().c_str()
                                   << bsoncxx::builder::stream::finalize);

    if (maybe_result)
    {
        bsoncxx::document::element emailElement = (*maybe_result).view()["activation_code"];

        activationCode = QString::fromStdString(emailElement.get_utf8().value.to_string());

    }
    else
    {
        return false;
    }

    return true;
}

bool UserManager::isValidEmail(const QString &email)
{
    if (email.size() > 32 || email.size() < 1 )
    {
        return false;
    }
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

void UserManager::generateActivationCode(const QString &email, QByteArray &activationCode)
{
    const unsigned int activationCodeSize = 256;
    QByteArray buffer = email.toLatin1();
    buffer.resize(activationCodeSize);
    unsigned int randomOffset = (email.size() < 32)?email.size():32;
    randombytes_buf((void *)(buffer.data() + randomOffset), activationCodeSize - randomOffset);
    QCryptographicHash hash(QCryptographicHash::Sha3_256);
    hash.addData(buffer);
    activationCode = hash.result().toBase64();

    qDebug() << activationCode;


}

#include "SessionManager.h"
#include <sodium.h>
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/bulk_write.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <QCryptographicHash>
#include <QDateTime>
#include "MongodbManager.h"
#include "SettingsManager.h"

SessionManager::SessionManager()
{

}

void SessionManager::init()
{
    auto client = MongodbManager::getSingleton().getClient();

    mongocxx::database swiftlyDb = (*client)["Swiftly"];
    mongocxx::collection sessionCollection = swiftlyDb["Session"];

    bsoncxx::builder::stream::document sessionIdIndexBuilder;
    mongocxx::options::index sessionIdOptions{};
    sessionIdIndexBuilder << "session_id" << 1;
    sessionIdOptions.unique(true);
    sessionCollection.create_index(sessionIdIndexBuilder.view(), sessionIdOptions);

}

bool SessionManager::newSession(const QString &userId, const QString &email, QByteArray &sessionId)
{
    auto client = MongodbManager::getSingleton().getClient();

    mongocxx::database swiftlyDb = (*client)["Swiftly"];
    mongocxx::collection sessionCollection = swiftlyDb["Session"];

    generateSessionId(sessionId);

    auto builder = bsoncxx::builder::stream::document{};
    bsoncxx::document::value sessionDocumentValue = builder
      << "email" << email.toStdString().c_str()
      << "session_id" << sessionId.toStdString().c_str()
      << "time" << static_cast<std::int64_t>( QDateTime::currentDateTimeUtc().toTime_t())
      << "user_id" << userId.toStdString().c_str()
      << bsoncxx::builder::stream::finalize;

    try
    {
        mongocxx::stdx::optional<mongocxx::result::insert_one> result =
        sessionCollection.insert_one(sessionDocumentValue.view());

        if (result)
        {
            return true;
        }
        else
        {
            //errorMessage = "Unknown issue when generate sessionId";
            return false;
        }
    }
    catch(const mongocxx::bulk_write_exception &e)
    {
        if (e.code().value() == 11000)
        {
            //qDebug() << "mongo error: duplicated email:" << e.what();
            //errorMessage = "Duplicated sessionId exists" % QString::fromLatin1(e.what());
        }
        else
        {
            //qDebug() << "unknown mongo error:" << e.code().value() << e.what();
            //errorMessage = "Database error:" % QString::fromLatin1(e.what());
        }
        return false;
    }

    return true;
}

bool SessionManager::findSession(const QByteArray &sessionId, QString &email)
{
    auto client = MongodbManager::getSingleton().getClient();

    mongocxx::database swiftlyDb = (*client)["Swiftly"];
    mongocxx::collection sessionCollection = swiftlyDb["Session"];

    mongocxx::stdx::optional<bsoncxx::document::value> maybe_result =
           sessionCollection.find_one(bsoncxx::builder::stream::document{}
                                   << "session_id" << QString::fromLatin1(sessionId).toStdString().c_str() << bsoncxx::builder::stream::finalize);

    if(maybe_result)
    {
        //std::cout << bsoncxx::to_json(*maybe_result) << "\n";
        //bsoncxx::document::element emailElement = (*maybe_result).view()["email"];
        //email =  QString::fromStdString(emailElement.get_utf8().value.to_string());

        bsoncxx::document::element timeElement = (*maybe_result).view()["time"];
        std::int64_t time = timeElement.get_int64().value;

        std::int64_t timeoutThreshold = SettingsManager::getSingleton().get("sessionTimeoutThreshold", 3600*24*7).toInt();

        std::int64_t currentTime = static_cast<std::int64_t>( QDateTime::currentDateTimeUtc().toTime_t());

        if ((currentTime - time) > timeoutThreshold)
        {
            //timeout

            mongocxx::stdx::optional<mongocxx::result::delete_result> result = sessionCollection.delete_one(bsoncxx::builder::stream::document{} << "session_id"
                                                                                                            << QString::fromLatin1(sessionId).toStdString().c_str()
                                                                                                            <<  bsoncxx::builder::stream::finalize);

            if (result)
            {

            }

            return false;
        }
        else
        {
            mongocxx::stdx::optional<mongocxx::result::update> result = sessionCollection.update_one(bsoncxx::builder::stream::document{} << "session_id"
                                                                                                  << QString::fromLatin1(sessionId).toStdString().c_str()
                                                                                                  << bsoncxx::builder::stream::finalize,
                                      bsoncxx::builder::stream::document{} << "$set"
                                      << bsoncxx::builder::stream::open_document
                                      << "time" << currentTime
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

        bsoncxx::document::element emailElement = (*maybe_result).view()["email"];

        email = QString::fromStdString(emailElement.get_utf8().value.to_string());


        return true;
    }
    else
    {
        //errorMessage = "No user registered with this email.";
        return false;
    }


    return true;
}

void SessionManager::generateSessionId(QByteArray &sessionId)
{
    const unsigned int sessionIdSize = 256;
    QByteArray buffer;
    buffer.resize(sessionIdSize);
    randombytes_buf((void *)buffer.data(), sessionIdSize);
    QCryptographicHash hash(QCryptographicHash::Sha3_256);
    hash.addData(buffer);
    sessionId = hash.result().toHex();
}

bool SessionManager::logoutSession(const QByteArray &sessionId)
{
    auto client = MongodbManager::getSingleton().getClient();

    mongocxx::database swiftlyDb = (*client)["Swiftly"];
    mongocxx::collection sessionCollection = swiftlyDb["Session"];

    mongocxx::stdx::optional<mongocxx::result::delete_result> maybe_result =
           sessionCollection.delete_one(bsoncxx::builder::stream::document{}
                                   << "session_id" << QString::fromLatin1(sessionId).toStdString().c_str() << bsoncxx::builder::stream::finalize);

    if(maybe_result)
    {
        return true;
    }
    else
    {
        //errorMessage = "No user registered with this email.";
        return false;
    }

    return true;
}

#ifndef MONGODBMANAGER_H
#define MONGODBMANAGER_H
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>


class MongodbManager
{
private:
    MongodbManager();
    mongocxx::instance m_instance;
    mongocxx::pool m_pool;

public:
    static MongodbManager& getSingleton()
    {
        static MongodbManager obj;
        return obj;
    }

    void init();
    mongocxx::pool::entry getClient();
};

#endif // MONGODBMANAGER_H

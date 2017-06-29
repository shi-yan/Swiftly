#include "MongodbManager.h"
#include <mongocxx/instance.hpp>

MongodbManager::MongodbManager()
    :m_instance{},
      m_pool{mongocxx::uri{}}
{

}

void MongodbManager::init(){}

mongocxx::pool::entry MongodbManager::getClient()
{
    return m_pool.acquire();
}

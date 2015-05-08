#include "HelloWorld.h"

void HelloWorld::registerPathHandlers()
{
   addGetHandler("/", "handleHelloWorldGet");
}

void HelloWorld::handleHelloWorldGet(HttpRequest &request, HttpResponse &response)
{
    response << "hello world from Swiftly!\n";
}

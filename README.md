# Swiftly

[![doctory badge](https://doctory.io/api/badge?ref_id=285b062063fa7a3c16f3bba99535119d66c9298cd94749a8f1a94046c08a060d "Doctory")](https://doctory.io/doc/shi-yan/Swiftly)

Swiftly is an easy to use Qt/C++ web framework I made out of boredom. As a C++ programmer, I sometimes feel left out when it comes to web development. All new languages are showing off their capabilities with some web development frameworks. But there seem to be less choices for C++ web frameworks. The goal of this project is to provide Qt programmers a way to implement web apps with their beloved framework. Swiftly emphasizes the implementation conciseness as well as speed.

## Installation:


## Usage:

Usually, A web site is consist of several distinct functionalities. Swiftly encourages developers to partition a web backend program into different modules based on functionality. For example, you could have a web site with a message board, a blog, a forum and a news update. These different functions or modules are called "web apps" by Swiftly's notion. When implementing a web site with Swiftly, each web app is enclosed inside a single "web app" class. The web site can have several web app classes to provide all the functionalities. Within each web app, you need to define a set of handler functions to handle requests sent to different paths. This idea should be familiar to any web backend developer. For example, you could have www.mysite.com/blog/page1, or www.mysite.com/blog/page2, these paths are handled by path handlers defined in web app classes.

Knowing these basic ideas, implementing a HelloWorld web app is as simple as defining a web app class and registering a route handler.

First, let's define a web app class "HelloWorld":

```cpp
#include "WebApp.h"

class HelloWorld : public WebApp
{
    Q_OBJECT

    public:
        void registerPathHandlers();

    public slots:
        void handleHelloWorldGet(HttpRequest &,HttpResponse &);
};
```

This class has two functions to implement. One is a function called registerPathHandlers(). Swiftly calls this function to let the web app to register path handlers.

All you need to do here is calling addGetHandler, telling it for which path (in this case "/"), call which function to handle it (in this case "handleHelloWorldGet").

```cpp
void HelloWorld::registerPathHandlers()
{
    AddGetHandler("/", handleHelloWorldGet);
}
```

The second function is the actual path handler which does the actual work, i.e return "Hello World" to the user.

```cpp
void HelloWorld::handleHelloWorldGet(HttpRequest &request, HttpResponse &response)
{
    response << "hello world from Swiftly!\n";
}
```

That's it! Oh, don't forget writing a main function:

```cpp
#include <QCoreApplication>
#include <QThread>
#include "Swiftly.h"
#include "HelloWorld.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    REGISTER_WEBAPP(HelloWorld);
    HttpServer::getSingleton().start(QThread::idealThreadCount(), 8080);
    return a.exec();
}
```

The main function is also very simple. There are two things you need to do. First, call REGISTER_WEBAPP with the name of your web app class. This will tell Swiftly that we will need to run this web app. Second, call HttpServer::getSingleton().start(QThread::idealThreadCount(), 8080). This launches the web server on port 8080. So if you go to a web browser and type http://localhost:8080, you should be able to see "hello world from Swiftly!"

# Swiftly

Swiftly is an easy to use Qt/C++ web framework. The goal is to provide Qt programmers a way to implement web apps with their beloved framework. Swiftly emphasizes the implementation conciseness as well as speed.

## Performance

Benchmark setup:

Intel(R) Core(TM) i7-5600U CPU @ 2.60GHz, 8GB memory, Ubuntu 15.04

### Benchmark of Serving hello world:

    ** SIEGE 3.0.8

    ** Preparing 200 concurrent users for battle.

    The server is now under siege...

    Lifting the server siege...      done.

1. Swiftly:
    

    Transactions:             206348 hits

    Availability:             100.00 %

    Elapsed time:               9.24 secs

    Data transferred:           2.56 MB

    Response time:              0.01 secs

    Transaction rate:       22332.04 trans/sec

    Throughput:             0.28 MB/sec

    Concurrency:              189.73

    Successful transactions:      206348

    Failed transactions:               0

    Longest transaction:            1.03

    Shortest transaction:           0.00

2. nodejs:

    ```javascript
    // Load the http module to create an http server.
    var http = require('http');
    
    // Configure our HTTP server to respond with Hello World to all requests.
    var server = http.createServer(function (request, response) {
      response.writeHead(200, {"Content-Type": "text/plain"});
      response.end("Hello World\n");
    });
    
    // Listen on port 8000, IP defaults to 127.0.0.1
    server.listen(8000);
    
    // Put a friendly message on the terminal
    console.log("Server running at http://127.0.0.1:8000/");
    ```
    
    Transactions:              89739 hits

    Availability:             100.00 %

    Elapsed time:               9.32 secs

    Data transferred:           1.03 MB

    Response time:              0.02 secs

    Transaction rate:        9628.65 trans/sec

    Throughput:             0.11 MB/sec

    Concurrency:              198.66

    Successful transactions:       89739

    Failed transactions:               0

    Longest transaction:            3.02

    Shortest transaction:           0.00


3. Python

    ```python
    from os import curdir, sep
    from BaseHTTPServer import BaseHTTPRequestHandler, HTTPServer
    
    class MyHandler(BaseHTTPRequestHandler):
    
        def do_GET(self):
            try:
                self.send_response(200)
                self.send_header('Content-type',    'text/html')
                self.end_headers()
                self.wfile.write('hello world')
    
            except IOError:
                self.send_error(404,'File Not Found: %s' % self.path)
    
    def main():
        try:
            server = HTTPServer(('', 8080), MyHandler)
            print 'Welcome to the machine...'
            server.serve_forever()
        except KeyboardInterrupt:
            print '^C received, shutting down server'
            server.socket.close()
    
    if __name__ == '__main__':
        main()
    ```

    Transactions:              39163 hits

    Availability:             100.00 %
    
    Elapsed time:               9.72 secs
    
    Data transferred:           0.41 MB
    
    Response time:              0.00 secs
    
    Transaction rate:        4029.12 trans/sec
    
    Throughput:             0.04 MB/sec
    
    Concurrency:               13.34
    
    Successful transactions:       39163
    
    Failed transactions:               0
    
    Longest transaction:            7.02
    
    Shortest transaction:           0.00

### Benchmark of Serving 1080p jpg image:

    $ siege -b -t10S -c200 http://localhost:8083/bench.jpg

1. Swiftly (without memory cache):
    
    Transactions:              10861 hits

    Availability:             100.00 %

    Elapsed time:               9.77 secs

    Data transferred:        1850.29 MB

    Response time:              0.17 secs

    Transaction rate:        1111.67 trans/sec

    Throughput:           189.38 MB/sec

    Concurrency:              192.31

    Successful transactions:       10861

    Failed transactions:               0

    Longest transaction:            1.31

    Shortest transaction:           0.00

2. nodejs static file server: http-server:
    
    Transactions:               9890 hits

    Availability:             100.00 %

    Elapsed time:               9.09 secs

    Data transferred:        2182.24 MB

    Response time:              0.18 secs

    Transaction rate:        1088.01 trans/sec

    Throughput:           240.07 MB/sec

    Concurrency:              197.34

    Successful transactions:        9890

    Failed transactions:               0

    Longest transaction:            1.33

    Shortest transaction:           0.10

3. Python one liner: python -m SimpleHTTPServer 8000:
    
    > Transactions:              17530 hits

    > Availability:             100.00 %

    Elapsed time:               9.84 secs

    Data transferred:        3868.01 MB

    Response time:              0.01 secs

    Transaction rate:        1781.50 trans/sec

    Throughput:           393.09 MB/sec

    Concurrency:                9.34

    Successful transactions:       17530

    Failed transactions:               0

    Longest transaction:            9.45

    Shortest transaction:           0.00

## A HelloWorld Example

A web site is usually consist of several distinct functionalities. For example, you could have a web site with a message board, a blog, a forum and a news update. These functions are called "web apps" by Swiftly's notion. When implementing a web site with Swiftly, each web app is enclosed inside a single web app class. The web site can have several web app classes to provide all the functionalities. For one web app, you need to handle different paths. For example, you could have www.mysite.com/blog/page1, or www.mysite.com/blog/page2, these paths are handled by path handlers in Swiftly.

Knowing these basic ideas, implementing a HelloWorld web app is as simple as defining a web app class and registering a path handler.

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

    ```C++
    void HelloWorld::registerPathHandlers()
    {
        addGetHandler("/", "handleHelloWorldGet");
    }
    ```

The second function is the actual path handler which does the actual work, i.e return "Hello World" to the user.

    ```c
    void HelloWorld::handleHelloWorldGet(HttpRequest &request, HttpResponse &response)
    {
        response << "hello world from Swiftly!\n";
    }
    ```

That's it! Oh, don't forget writing a main function:

    ```c++
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

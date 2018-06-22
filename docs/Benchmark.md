## Benchmark

Performance is not yet a focus of Swiftly, but it is good to know where it stands against other options. I don't have the energy to implement similar server using different frameworks. So here I just use the naive hello world server. The hello world server using Swiftly is included in the Example folder, whereas for the nodejs, python and golang servers are documented here:

1. Swiftly:
```cpp
#include "WebApp.h"

class HelloWorld : public WebApp
{
    Q_OBJECT

public:
    void registerPathHandlers() override;

public slots:
    void handleHelloWorldGet(HttpRequest &,HttpResponse &);
};

void HelloWorld::registerPathHandlers()
{
    AddGetHandler("/", handleHelloWorldGet);
}

void HelloWorld::handleHelloWorldGet(HttpRequest &request, HttpResponse &response)
{
    response << "hello world!";
}
```

2. nodejs:
```javascript
var http = require('http');
http.createServer(function (req, res) {
    res.writeHead(200, {'Content-Type': 'text/plain'});
    res.end('hello world!');
}).listen(8124, "127.0.0.1");
console.log('Server running at http://127.0.0.1:8124/');
```

3. python3:
```python
#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from http.server import HTTPServer
from http.server import BaseHTTPRequestHandler
from http import HTTPStatus

class MyHandler(BaseHTTPRequestHandler):

    def do_GET(self):
        self.send_response(HTTPStatus.OK)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        self.wfile.write(b'hello world!')
        return


def run(server_class=HTTPServer, handler_class=MyHandler):
    server_address = ('localhost', 8000)
    httpd = server_class(server_address, handler_class)
    try:
        print("Server works on http://localhost:8000")
        httpd.serve_forever()
    except KeyboardInterrupt:
        print("Stop the server on http://localhost:8000")
        httpd.socket.close()


if __name__ == '__main__':
    run()
```

The benchmark tool I used was the apache benchmark. I issued 10000 requests with 250 concurrent threads. I have noticed some fluctuation between runs, so I ran the same command for multiple times to get stable results.



It might be unfair to launch many Swiftly workers and compare its performance against the nodejs server, as we know that javascript is single threaded. But who knows if nodejs runtime would launch other threads to help with the event loop? So I think it may be worthwhile to run the same benchmark using just one cpu core.
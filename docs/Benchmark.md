## Benchmark

Performance is not yet a focus of Swiftly, but it is good to know where it stands against other options. I don't have the energy to implement similar servers using different frameworks. So here I just use the naive hello world server. The hello world server using Swiftly is included in the Example folder, whereas for the nodejs and golang servers are documented here. I'd also include a python3 server, but it performed really bad compared to other servers. It constant caused broken pipe issues or timeout issues. I think it's meaningless to include it.

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

3. golang:
```golang
package main
import (
	"net/http"
	"io"
)

func helloHandler(w http.ResponseWriter, r *http.Request) {
	io.WriteString(w, "hello world!")
}


func main() {
	http.HandleFunc("/", helloHandler)
	http.ListenAndServe(":8080", nil)
}
```


The benchmark tool I used was the apache benchmark. I issued 10000 requests with 250 concurrent threads. I have noticed some fluctuation between runs, especially for the nodejs server for the first few launches. It appears to require some warm-up. so I ran the same command for multiple times to get stable results.

The command line:
> ab -n 10000 -c 250 -q {url}

The results can be summarized by the following tables:

#### Requests per sec (#/sec): 

| Swiftly        | nodejs           | golang  |
| :-------------: |:-------------:| :-----:|
| 15658.4      | 10508.73 | 15935.57 |
| 15881.44      | 10604.9   |   14839.13 |
| 15663.06     | 10071.75      |    15883.1 |
| 15749.32 | 10196.2     |    15443.75 |

![Requests per sec](benchmark_rps.png "Requests per sec (#/sec)")


#### Time per request (ms):

| Swiftly| nodejs | golang |
| :-------------: |:-------------:| :-----:|
| 15.966 | 23.79 | 15.688 |
| 15.742 | 23.574 |	16.847 |
| 15.961 | 24.822 | 15.74 |
| 15.874 | 24.519 | 16.188 |

![Time per request](benchmark_tpr.png "Time per request (ms)")


#### Transfer rate (kbyte/sec) :

| Swiftly | nodejs | golang |
| :-------------: |:-------------:| :-----:|
| 1422.1 | 1159.65 | 2007.51 |
| 1422.36 | 1170.27 | 1869.38 |
| 1422.52 | 1111.43 | 2000.9 |
| 1430.36 | 1125.17 | 1945.55 |

![Transfer rate](benchmark_tr.png "Transfer rate (kbyte/sec)")


It might be unfair to launch many Swiftly workers and compare its performance against the nodejs server, as we know that javascript is single-threaded. But who knows if nodejs runtime would launch other threads to help with the event loop? So I think it may be worthwhile to run the same benchmark using just one cpu core:

> taskset -c 0 {server launch command}

#### Requests per sec (#/sec) [Single thread]:

| Swiftly | nodejs | golang |
| :-------------: |:-------------:| :-----:|
| 11461.04 | 10527.44 | 15470.37 |
| 11178.8 | 10450.59 | 15833.63 |
| 11477.22 | 10598.71 | 15470.58 |
| 11376.84 | 10454.06 | 15888.07 |

![Requests per sec](benchmark_single_rps.png "Requests per sec (#/sec) [Single thread]")


#### Time per request (ms) [Single thread]:

| Swiftly | nodejs | golang|
| :-------------: |:-------------:| :-----:|
| 21.813 | 23.747 | 16.16 |
| 22.364 | 23.922 | 15.789 |
| 21.782 | 23.588 | 16.16 |
| 21.974 | 23.914 | 15.735 |

![Time per request](benchmark_single_tpr.png "Time per request (ms) [Single thread]")


#### Transfer rate (kbyte/sec) [Single thread]:

| Swiftly | nodejs | golang |
| :-------------: |:-------------:| :-----:|
| 1040.9 | 1161.72 | 1948.9|
| 1015.26 | 1153.24 | 1994.67|
| 1042.36 | 1169.58 | 1948.93|
| 1033.25 | 1153.62 | 2001.52|

![Transfer rate](benchmark_single_tr.png "Transfer rate (kbyte/sec) [Single thread]")

### cpu info as printed by /proc/cpuinfo (6 cores 12 threads):

> cpu family	: 6  
> model		: 45  
> model name	: Intel(R) Core(TM) i7-3930K CPU @ 3.20GHz  
> cpu MHz		: 1940.128  
> cache size	: 12288 KB  
> siblings	: 12  
> cpu cores	: 6

### Conclusion
As you can see, Swiftly is comparable with (slightly better than) nodejs in both multi-thread and single-thread mode, whereas the golang server seems to be the best among the three for now, especially for the single-thread case. In the multi-thread case, Swiftly's performance is around the same level as that of the golang server.

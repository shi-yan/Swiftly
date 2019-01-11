#ifndef ADMINPAGECONTENT_H
#define ADMINPAGECONTENT_H

#include <string>

static std::string tem = R"(
<html>
<head>
<title>Swiftly Admin Console</title>
</head>
<body>
<h2>Swiftly Admin Console</h2>
<button onclick='shutdown()' id='shutdownbutton'>Shutdown</button>
<script>
    function shutdown()
    {
        fetch('/console?cmd=shutdown')
          .then(function(response){
            console.log('done!')
            document.getElementById('shutdownbutton').innerText = 'Shutdown done!'
          })
    }
</script>
</body>
</html>
)";

#endif // ADMINPAGECONTENT_H

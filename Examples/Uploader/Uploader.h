#ifndef UPLOADER_H
#define UPLOADER_H
#include "WebApp.h"

class Uploader : public WebApp
{
Q_OBJECT

public:
    void registerPathHandlers() override;
    Uploader();

public slots:
    void handleUploadFormGet(HttpRequest &,HttpResponse &);
    void handleUploadPost(HttpRequest &, HttpResponse &);
};

#endif // UPLOADER_H

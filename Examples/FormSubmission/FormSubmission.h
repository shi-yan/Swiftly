#ifndef FORMSUBMISSION_H
#define FORMSUBMISSION_H
#include "WebApp.h"

class FormSubmission : public WebApp
{
Q_OBJECT

public:
    void registerPathHandlers() override;
    FormSubmission();

public slots:
    void handleUploadFormGet(HttpRequest &,HttpResponse &);
    void handleUploadPost(HttpRequest &, HttpResponse &);
};

#endif // FORMSUBMISSION_H

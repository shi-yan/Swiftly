#ifndef FORMSUBMISSION_H
#define FORMSUBMISSION_H
#include "WebApp.h"

class FormData
{
public:
    QString m_fieldName;
    QString m_meta;
    QByteArray m_data;

    FormData(const QString &meta = "", const QByteArray &data= QByteArray()):
        m_meta(meta),
        m_data(data){}
};

class FormSubmission : public WebApp
{
Q_OBJECT

public:
    void registerPathHandlers() override;
    FormSubmission();

protected:

    bool parseFormData(const QByteArray &rawData, const QString &boundary, QVector<FormData> &realContent);

public slots:
    void handleUploadFormGet(HttpRequest &,HttpResponse &);
    void handleUploadPost(HttpRequest &, HttpResponse &);
};

#endif // FORMSUBMISSION_H

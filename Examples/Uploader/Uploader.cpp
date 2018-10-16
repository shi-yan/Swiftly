#include "Uploader.h"
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStringBuilder>
#include <QFile>

Uploader::Uploader()
{

}

void Uploader::registerPathHandlers()
{
    AddGetHandler("/", handleUploadFormGet);
    AddPostHandler("/upload", handleUploadPost);
}

void Uploader::handleUploadFormGet(HttpRequest &request, HttpResponse &response)
{
    response.setStatusCode(200);
    response << "<html><head><title>upload test</title></head><body><form enctype='multipart/form-data' method='post' action='/upload' ><label for='fileupload'>Choose A File to upload</label><input type='file' name='fileupload' value='fileupload' id='fileupload'></input><input type='submit' value='submit' /></form></body></html>";
    response.finish();
}

void Uploader::handleUploadPost(HttpRequest &request, HttpResponse &response)
{

    if (request.hasFormData())
    {
        QWeakPointer<HttpRequest::FormData> formData = request.getFormData("fileupload");
        if (!formData.isNull())
        {
            if (formData.data()->m_fields.contains("Content-Disposition"))
            {
                if(formData.data()->m_fields["Content-Disposition"].contains("filename"))
                {
                    QString filename = formData.data()->m_fields["Content-Disposition"]["filename"];

                    if (formData.data()->m_fields.contains("Content-Type"))
                    {
                        QString contentType = *(formData.data()->m_fields["Content-Type"].begin());

                        QString newFilename = "uploaded_" % filename;

                        QFile file(newFilename);

                        if (file.open(QFile::WriteOnly))
                        {
                            file.write(formData.data()->m_data);
                            file.close();
                            response.setStatusCode(200);
                            response << "uploaded\n" << filename <<"\n"<< contentType;
                            response.finish();
                            return;
                        }
                    }
                }
            }
        }
    }

    response.setStatusCode(400);
    response << "upload failed";
    response.finish();
}

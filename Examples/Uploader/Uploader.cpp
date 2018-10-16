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
    response << "<html>"
                "<head>"
                "<title>upload test</title>"
                "</head>"
                "<body>"
                "<form enctype='multipart/form-data' method='post' action='/upload'>"
                "<label for='fileupload'>Choose A File to upload</label><br />"
                "<input type='file' accept='image/*' name='fileupload' multiple='multiple' value='fileupload' id='fileupload'>"
                "</input><br />"
                "<input type='submit' value='submit' />"
                "</form>"
                "</body>"
                "</html>";
    response.finish();
}

void Uploader::handleUploadPost(HttpRequest &request, HttpResponse &response)
{

    qDebug() << request.getRawData();

    if (request.hasFormData())
    {
        QVector<QSharedPointer<HttpRequest::FormData>> formData = request.getFormData("fileupload");
        if (formData.size())
        {
            response.setStatusCode(200);
            for(int i=0;i<formData.size();++i)
            {
                if (formData[i].data()->m_fields.contains("Content-Disposition"))
                {
                    if(formData[i].data()->m_fields["Content-Disposition"].contains("filename"))
                    {
                        QString filename = formData[i].data()->m_fields["Content-Disposition"]["filename"];

                        if (formData[i].data()->m_fields.contains("Content-Type"))
                        {
                            QString contentType = *(formData[i].data()->m_fields["Content-Type"].begin());

                            QString newFilename = "uploaded_" % filename;

                            QFile file(newFilename);

                            if (file.open(QFile::WriteOnly))
                            {
                                file.write(formData[i].data()->m_data);
                                file.close();
                                response << "uploaded: " << filename <<" "<< contentType << "<br />";
                            }
                            else
                            {
                                response << "failed to save: " << filename <<" "<< contentType << "<br />";
                            }
                        }
                        else
                        {
                            response << "error: no Content-Type <br/>";
                        }
                    }
                    else
                    {
                        response << "error: no filename <br/>";
                    }
                }
                else
                {
                    response << "error: no Content-Disposition <br/>";
                }
            }

            response.finish();
            return;
        }
    }

    response.setStatusCode(400);
    response << "upload failed";
    response.finish();
}

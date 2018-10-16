#include "FormSubmission.h"
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStringBuilder>

FormSubmission::FormSubmission()
{

}

void FormSubmission::registerPathHandlers()
{
    AddGetHandler("/", handleUploadFormGet);
    AddPostHandler("/upload", handleUploadPost);
}

void FormSubmission::handleUploadFormGet(HttpRequest &request, HttpResponse &response)
{
    response.setStatusCode(200);
    response << "<html>"
                "<head>"
                "<title>upload test</title>"
                "</head>"
                "<body>"
                "<form enctype='multipart/form-data' method='post' action='/upload' ><fieldset>"
                "<label for='textfield1'>Choose A File to upload</label><br />"
                "<input type='text' name='textfield1' value='textfield1' id='textfield1'></input><br />"
                "<label for='textfield2'>Choose A File to upload</label><br />"
                "<input type='text' name='textfield2' value='textfield2' id='textfield2'></input></fieldset><br />"
                "<input type='radio' name='gender' value='male' checked /> Male<br />"
                "<input type='radio' name='gender' value='female' /> Female<br />"
                "<input type='radio' name='gender' value='other' /> Other <br />"
                "<input type='checkbox' name='vehicle1' value='Bike' /> I have a bike<br />"
                "<input type='checkbox' name='vehicle2' value='Car'> I have a car <br />"
                "<input type='submit' value='submit' />"
                "</form>"
                "</body>"
                "</html>";
    response.finish();
}

void FormSubmission::handleUploadPost(HttpRequest &request, HttpResponse &response)
{
    if (request.hasFormData())
    {
        response.setStatusCode(200);
        response << "form submitted: <br />";
        for (auto iter = request.getFormData().begin();iter != request.getFormData().end();++iter)
        {
            QString item = iter.key() % " = " % iter.value()[0].data()->m_data % "; <br />";
            response << item;
        }
        response.finish();
    }
}

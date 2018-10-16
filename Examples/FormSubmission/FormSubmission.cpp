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
/*
static bool detectSeparator(const QString &separator, const QByteArray &rawData, int &index)
{
    for(int i =0;i< separator.length();++i)
    {
        if (index >= rawData.size())
        {
            return false;
        }

        if (separator[i] != rawData.data()[index++])
        {
            return false;
        }
    }
    return true;
}
*/
class PatternTracer
{
private:
    class Tracker
    {
    public:
        int m_start;
        int m_pointer;

        Tracker(int start = 0, int pOffset = 0)
            :m_start(start),m_pointer(pOffset){}
    };

    QString m_pattern;
    QVector<Tracker> m_trackerList;
    int m_index;
    int m_pOffset;
public:
    PatternTracer(const QString &pattern, int index, int pOffset = 0)
        :m_pattern(pattern),
          m_trackerList(),
          m_index(index),
          m_pOffset(pOffset)
    {}

    bool push(char c, int &start, int &end)
    {
        for(int i =0;i<m_trackerList.size();)
        {
            if (c == m_pattern[m_trackerList[i].m_pointer+1])
            {
                m_trackerList[i].m_pointer++;
                if (m_trackerList[i].m_pointer == m_pattern.size()-1)
                {
                    start = m_trackerList[i].m_start;
                    end = m_trackerList[i].m_pointer + 1 + start - m_pOffset;

                    return true;
                }
                ++i;
            }
            else
            {
                m_trackerList[i] = m_trackerList[m_trackerList.size() - 1];
                m_trackerList.pop_back();
            }
        }


        if (c == m_pattern[m_pOffset])
        {
            Tracker t(m_index, m_pOffset);
            m_trackerList.push_back(t);
        }
        m_index++;
        return false;
    }
};


bool FormSubmission::parseFormData(const QByteArray &rawData, const QString &boundary, QVector<FormData> &realContent)
{
    /*enum class State
    {
        START,
        CONTENT,
        END,
    };*/

    //State curState = State::START;

    QString separator = "\r\n--" % boundary % "\r\n";
    QString ending = "\r\n--" % boundary % "--\r\n";
    QString realData = "\r\n\r\n";

    int begin = 0;
    int prevBoundary = -1;
    int prevRealDataBoundary = -1;

    while(begin < rawData.size())
    {
        PatternTracer sepTracer(separator, begin, begin==0?2:0);
        PatternTracer endTracer(ending, begin);
        PatternTracer realDataTracer(realData, begin);
        prevRealDataBoundary = -1;

        int i = begin;

        while(i < rawData.size())
        {
            char c = rawData.data()[i];

            int start = 0;
            int end = 0;

            if (endTracer.push(c, start, end))
            {
                if (prevBoundary != -1 && end == rawData.size() && prevRealDataBoundary != -1 && prevRealDataBoundary + realData.length() <= start)
                {
                    QByteArray metaData(rawData.data() + prevBoundary, prevRealDataBoundary - prevBoundary);

                    QString meta = QString::fromUtf8(metaData);

                    QByteArray actualData(rawData.data() + prevRealDataBoundary + realData.length(), start - (prevRealDataBoundary+realData.length()));

                    realContent.push_back(FormData(meta, actualData));

                    return true;
                }
                return false;
            }
            else if(sepTracer.push(c, start, end))
            {
                if (prevBoundary != -1 && prevRealDataBoundary != -1 && prevRealDataBoundary+realData.length() <= start)
                {
                    QByteArray metaData(rawData.data() + prevBoundary, prevRealDataBoundary - prevBoundary);

                    QString meta = QString::fromUtf8(metaData);

                    QByteArray actualData(rawData.data() + prevRealDataBoundary + realData.length(), start - (prevRealDataBoundary+realData.length()));

                    realContent.push_back(FormData(meta, actualData));
                }

                begin = prevBoundary = end;
                break;
            }
            else if (realDataTracer.push(c, start, end))
            {
                if (prevRealDataBoundary == -1)
                {
                    prevRealDataBoundary = start;
                }
            }
            ++i;
        }
    }

    return false;
}

void FormSubmission::handleUploadPost(HttpRequest &request, HttpResponse &response)
{
    qDebug() << "upload";
    qDebug() << request.getHeader().toString();

    qDebug() << request.getRawData();

    qDebug() << *(request.getHeader().getHeaderInfo("Content-Type").data());

    QString contentType = *(request.getHeader().getHeaderInfo("Content-Type").data());

    QRegularExpression boundaryRegExp("^multipart/form-data; boundary=([0-9a-zA-Z'()+-_,./:=? ]{0,69}[0-9a-zA-Z'()+-_,./:=?]{1})$");

    qDebug() <<"valid:"<< boundaryRegExp.isValid();

    QRegularExpressionMatch match = boundaryRegExp.match(contentType);

    if (!match.hasMatch())
    {
        response.setStatusCode(400);
        response << "unable to understand content-type";
        response.finish();
    }

    QString boundary = match.captured(1);
    qDebug() << "has match" << boundary;

    //QString separator = "--" % boundary;
    //QString ending = "--" % boundary % "--";

    QByteArray rawData = request.getRawData();

    QVector<FormData> realContent;

    if (parseFormData(rawData, boundary, realContent))
    {
        qDebug() << "cool passed";
        for(int i =0;i<realContent.size();++i){
            qDebug() << realContent[i].m_meta << realContent[i].m_data;
        }
    }
    else
    {
        qDebug() << "not pass";
    }

    response.setStatusCode(200);
    response << "uploaded";
    response.finish();
}

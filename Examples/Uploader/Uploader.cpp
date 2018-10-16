#include "Uploader.h"
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStringBuilder>

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

        Tracker(int start = 0)
            :m_start(start),m_pointer(0){}
    };

    QString m_pattern;
    QVector<Tracker> m_trackerList;
    int m_index;
public:
    PatternTracer(const QString &pattern, int index)
        :m_pattern(pattern),
          m_trackerList(),
          m_index(index){}

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
                    end = m_trackerList[i].m_pointer + 1 + start;

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


        if (c == m_pattern[0])
        {
            Tracker t(m_index);
            m_trackerList.push_back(t);
        }
        m_index++;
        return false;
    }
};


bool Uploader::parseFormData(const QByteArray &rawData, const QString &boundary, QVector<FormData> &realContent)
{
    /*enum class State
    {
        START,
        CONTENT,
        END,
    };*/

    //State curState = State::START;

    QString separator = "--" % boundary % "\r\n";
    QString ending = "\r\n--" % boundary % "--\r\n";
    QString realData = "\r\n\r\n";

    int begin = 0;
    int prevBoundary = -1;
    int prevRealDataBoundary = -1;

    while(begin < rawData.size())
    {
        PatternTracer sepTracer(separator, begin);
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

void Uploader::handleUploadPost(HttpRequest &request, HttpResponse &response)
{
    qDebug() << "upload";
    qDebug() << request.getHeader().toString();

    //qDebug() << request.getRawData();

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
        qDebug() << "cool passed" << realContent[0].m_meta << realContent[0].m_data;
    }
    else
    {
        qDebug() << "not pass";
    }

    response.setStatusCode(200);
    response << "uploaded";
    response.finish();
}

#ifndef TASKHANDLER_H
#define TASKHANDLER_H

#include <QObject>
#include <QMetaMethod>
#include "HttpRequest.h"
#include "HttpResponse.h"

class TaskHandler:public QObject
{
    Q_OBJECT

    QObject *object;
    int methodID;

public:
    TaskHandler( QObject *parent=0);
    TaskHandler(const TaskHandler &in);

    void operator=(const TaskHandler &in);
    bool isEmpty() const;
    bool setHandler(QObject *_object,const QString &methodName);

    const QObject *getObject() const
    {
        return object;
    }

     int getMethodID()
    {
        return methodID;
    }

    bool invoke(HttpRequest &request,HttpResponse &response) const
    {
        if(object && methodID!=-1)
        {
            return object->metaObject()->method(methodID).invoke(object,Qt::DirectConnection,Q_ARG(HttpRequest&,request),Q_ARG(HttpResponse&,response));
        }
        else
            return false;
    }
};

#endif // TASKHANDLER_H

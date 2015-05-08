#include "TaskHandler.h"



TaskHandler::TaskHandler( QObject *parent)
    :QObject(parent),
      object(0),
      methodID(-1)
{
}

TaskHandler::TaskHandler(const TaskHandler &in)
    :QObject(),
      object(in.object),
      methodID(in.methodID)
{
}

void TaskHandler::operator=(const TaskHandler &in)
{
    object=in.object;
    methodID=in.methodID;
}

bool TaskHandler::isEmpty() const
{
    return object==0;
}

bool TaskHandler::setHandler(QObject *_object,const QString &methodName)
{
    methodID=_object->metaObject()->indexOfMethod(methodName.toStdString().c_str());

    if(methodID!=-1)
    {
        object=_object;
        return true;
    }
    else
        return false;
}

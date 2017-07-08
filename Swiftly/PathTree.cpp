#include "PathTree.h"
#include <QDebug>

PathTree::PathTree(QObject *parent)
    :QObject(parent),
      root("")
{
}

bool PathTree::registerAPath(const QString &path,QObject *object,const QString &methodName,enum PathTreeNode::TaskHandlerType type)
{
    /*qDebug()<<"Register a Handler:";
    qDebug()<<"Path:"<<path;
    qDebug()<<"Object:"<<object;
    qDebug()<<"Method Name:"<<methodName;
    //qDebug()<<"Method Count:"<<((Zoidblog*)object)->metaObject()->methodCount();
    //qDebug()<<"Method Signature:"<<((Zoidblog*)object)->metaObject()->method(4).signature();
    qDebug()<<"Method ID:"<<object->metaObject()->indexOfMethod(QMetaObject::normalizedSignature(methodName.toStdString().c_str()));
    */

    if(!path.isNull() && !path.isEmpty() && object!=0 && object->metaObject()->indexOfMethod(methodName.toStdString().c_str())!=-1 && !methodName.isNull() && !methodName.isEmpty())
    {
        if(path.at(0)!='/')
            return false;
        else if(path.count()==1)
        {
            if(type==PathTreeNode::GET)
                return root.setGetHandler(object,methodName);
            else
                return root.setPostHandler(object,methodName);
        }
        else
        {
            int posBegin=1;
            int posEnd=1;

            PathTreeNode *currentPathTreeNode=&root;

            for(posEnd=1;posEnd<path.count();++posEnd)
            {
                if(path.at(posEnd)=='/' || posEnd==path.count()-1)
                {
                    if(posEnd==path.count()-1)
                        posEnd++;

                    QString pathName=path.mid(posBegin,posEnd-posBegin);

                    if(!currentPathTreeNode->hasChild(pathName))
                    {
                        currentPathTreeNode->addChild(pathName);
                    }

                    currentPathTreeNode=currentPathTreeNode->getChild(pathName);

                    posEnd=posBegin=posEnd+1;
                }
            }

            if(type==PathTreeNode::GET)
                return currentPathTreeNode->setGetHandler(object,methodName);
            else
                return currentPathTreeNode->setPostHandler(object,methodName);
        }
    }
    else
        return false;
}

const TaskHandler * PathTree::getTaskHandlerByPath(const QString &path,enum PathTreeNode::TaskHandlerType type)
{
    if(!path.isNull() && !path.isEmpty() && path.at(0)=='/')
    {
        if(path.count()==1)
        {
            if(type==PathTreeNode::GET)
                return root.getHandler();
            else
                return root.postHandler();
        }
        else
        {
            int posBegin=1;
            int posEnd=1;

            PathTreeNode *currentPathTreeNode=&root;

            for(posEnd=1;posEnd<path.count();++posEnd)
            {
                if(path.at(posEnd)=='/' || posEnd==path.count()-1)
                {
                    if(posEnd==path.count()-1)
                        posEnd++;

                    QString pathName=path.mid(posBegin,posEnd-posBegin);
                    //qDebug() << "pathName" << pathName;

                    if(currentPathTreeNode->hasChild(pathName))
                    {
                        currentPathTreeNode=currentPathTreeNode->getChild(pathName);
                        posBegin = posEnd = posEnd + 1;
                    }
                    else
                    {
                        break;
                    }
                }
            }


            if(type==PathTreeNode::GET)
                return currentPathTreeNode->getHandler();
            else
                return currentPathTreeNode->postHandler();
        }
    }
    else
        return 0;
}

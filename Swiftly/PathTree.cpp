#include "PathTree.h"
#include <QDebug>

PathTree::PathTree(QObject *parent)
    :QObject(parent),
      m_root("")
{
}

bool PathTree::registerAPath(const QString &path,QObject *object,const QString &methodName,enum PathTreeNode::HttpVerb verb)
{
    /*qDebug()<<"Register a Handler:";
    qDebug()<<"Path:"<<path;
    qDebug()<<"Object:"<<object;
    qDebug()<<"Method Name:"<<methodName;
    //qDebug()<<"Method Count:"<<((Zoidblog*)object)->metaObject()->methodCount();
    //qDebug()<<"Method Signature:"<<((Zoidblog*)object)->metaObject()->method(4).signature();
    qDebug()<<"Method ID:"<<object->metaObject()->indexOfMethod(QMetaObject::normalizedSignature(methodName.toStdString().c_str()));
    */

    if(!path.isEmpty() && object!=nullptr && object->metaObject()->indexOfMethod(methodName.toStdString().c_str())!=-1 && !methodName.isEmpty())
    {
        if(path.at(0)!='/')
            return false;
        else if(path.count()==1)
        {
            if(verb==PathTreeNode::GET)
                return m_root.setGetHandler(object, methodName);
            else
                return m_root.setPostHandler(object, methodName);
        }
        else
        {
            int posBegin=1;
            int posEnd=1;

            PathTreeNode *currentPathTreeNode = &m_root;

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

            if(verb==PathTreeNode::GET)
                return currentPathTreeNode->setGetHandler(object,methodName);
            else
                return currentPathTreeNode->setPostHandler(object,methodName);
        }
    }
    else
        return false;
}

const TaskHandler * PathTree::getTaskHandlerByPath(const QString &path,enum PathTreeNode::HttpVerb type)
{
    if(!path.isEmpty() && path.at(0)=='/')
    {
        if(path.count()==1)
        {
            if(type==PathTreeNode::GET)
                return m_root.getHandler();
            else
                return m_root.postHandler();
        }
        else
        {
            int posBegin=1;
            int posEnd=1;

            PathTreeNode *currentPathTreeNode = &m_root;

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

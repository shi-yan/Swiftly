#ifndef PATHTREE_H
#define PATHTREE_H

#include <QObject>
#include "PathTreeNode.h"

/*! \brief PathTree represents a tree strcture that cat route a request to its handler
 *
 * Each http request has a route. The entire route structure is saved in a tree form
 * in this class.
 */
class PathTree : public QObject
{
    Q_OBJECT


    //! the root of the route path tree
    PathTreeNode m_root;

public:
    /*!
     * \brief PathTree constructor
     * \param[in] parent the parent Qt object.
     */
    PathTree(QObject *parent = nullptr);

    /*!
     * \brief registerAPath parse a path and merge it into the current route tree
     * \param[in] path is the route path
     * \param[in] object is the WebApp that will handle the requests of this path
     * \param[in] methodName the name of the function of the WebApp that should handle this request
     * \param[in] verb the verb of the handler response to, such as, GET or POST
     * \return return true on sucess.
     *
     * After implementing a WebApp, call this function to tell Swiftly that certain http request should be handled by
     * which WebApp and which function. This function will parse the path provided by using "/" as a separator and merge
     * the result into the internal path tree. At each leaf node of the tree, there is a QObject, which is the WebApp and a
     * handler function. This handler function will be triggered once Swiftly receives a http request of the registered path.
     *
     * There can't be 2 WebApps that share the same route. If a WebApp has been registered to handle a route, another registration
     * using the same path will fail.
     */
    bool registerAPath(const QString &path, const std::function<void(HttpRequest &, HttpResponse &)> &in, enum PathTreeNode::HttpVerb verb);

    /*!
     * \brief getTaskHandlerByPath this function returns a task handler for certain path and handle type.
     * \param[in] path the request route
     * \param[in] taskHandleType the type of the handler, such as GET or POST
     * \return return the handler's pointer upon finish.
     *
     * \todo HttpVerb should be a strong type enum
     */
    const std::function<void(HttpRequest &, HttpResponse &)> & getTaskHandlerByPath(const QString &path, enum PathTreeNode::HttpVerb verb);
};

#endif // PATHTREE_H

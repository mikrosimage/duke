#ifndef DKXNODEMANAGER_H
#define DKXNODEMANAGER_H

#include "dkxINode.h"
#include "dkxSession.h"
#include "dkxObservable.h"
#include <map>

class NodeManager {

public:
    typedef boost::shared_ptr<NodeManager> ptr;

public:
    NodeManager(){}

public:
    bool addNode(INode::ptr _node, Session::ptr _session);
    bool removeNode(const std::string & _nodename);
    void clearNodes();
    void setSessionOnNodes(Session::ptr _session);

    template<class TYPE>
    boost::shared_ptr<TYPE> nodeByName(const std::string & _nodename) {
        INode::ptr n = findNode(_nodename);
        if(n.get() != NULL)
            return boost::dynamic_pointer_cast<TYPE>(n);
        return boost::shared_ptr<TYPE>();
    }

private:
    NodeManager(const NodeManager&);
    const NodeManager& operator=(const NodeManager&);
    INode::ptr findNode(const std::string & _nodename);

private:
    typedef std::map<std::string, INode::ptr> NodeMap;
    typedef std::pair<std::string, INode::ptr> NodePair;
    NodeMap mNodes;
};

#endif // DKXNODEMANAGER_H

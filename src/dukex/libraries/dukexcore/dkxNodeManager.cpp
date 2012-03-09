#include "dkxNodeManager.h"
#include <iostream>

//using namespace ::duke::protocol;
using namespace std;

bool NodeManager::addNode(INode::ptr _node, Session::ptr _session) {
    if(!_node)
        return false;
    NodeMap::const_iterator it = mNodes.find(_node->name());
    if (it != mNodes.end())
        return false;
    _node->setSession(_session);
    mNodes[_node->name()] = _node;
    return true;
}

bool NodeManager::removeNode(const std::string & _nodename) {
    NodeMap::iterator it = mNodes.find(_nodename);
    if (it == mNodes.end())
        return false;
    mNodes.erase(it);
    return true;
}

void NodeManager::clearNodes() {
    mNodes.clear();
}

void NodeManager::setSessionOnNodes(Session::ptr _session) {
    NodeMap::iterator it = mNodes.begin();
    while(it != mNodes.end()){
        it->second->setSession(_session);
    }
}

// private
INode::ptr NodeManager::findNode(const std::string & _nodename) {
    NodeMap::const_iterator it = mNodes.find(_nodename);
    if (it != mNodes.end())
        return it->second;
    std::cerr << "Error: Unable to find node "<< _nodename << std::endl;
    INode::ptr p;
    return p;
}

#ifndef DKXINODE_H
#define DKXINODE_H

#include "dkxSession.h"
#include <boost/shared_ptr.hpp>

class INode {

public:
    typedef boost::shared_ptr<INode> ptr;
    INode(const std::string & _name) :
        mName(_name) {
    }
    virtual ~INode() {
    }

public:
    inline const std::string & name() const {
        return mName;
    }
    inline Session::ptr session() {
        return mSession;
    }
    inline void setSession(Session::ptr _session) {
        mSession = _session;
    }

private:
    std::string mName;
    Session::ptr mSession;
};

#endif // DKXINODE_H

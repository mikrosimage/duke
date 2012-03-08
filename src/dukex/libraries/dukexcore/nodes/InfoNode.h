#ifndef INFONODE_H
#define INFONODE_H

#include <dukexcore/dkxINode.h>


class InfoNode : public INode {

public:
    typedef boost::shared_ptr<InfoNode> ptr;

public:
    InfoNode() :
        INode("fr.mikrosimage.dukex.info") {
    }

public:
    void callCurrentImageInfo() {
        MessageQueue queue;
        ::duke::protocol::Info info;
        info.set_content(::duke::protocol::Info_Content_IMAGEINFO);
        push(queue, info, google::protobuf::serialize::MessageHolder::RETRIEVE);
        session()->sendMsg(queue);
    }

    void callCurrentCacheState() {
        MessageQueue queue;
        ::duke::protocol::Info info;
        info.set_content(::duke::protocol::Info_Content_CACHESTATE);
        push(queue, info, google::protobuf::serialize::MessageHolder::RETRIEVE);
        session()->sendMsg(queue);
    }

};

#endif // INFONODE_H

#include "dkxSession.h"
#include <dukeapi/SocketMessageIO.h>
#include <dukeapi/messageBuilder/Commons.h>
#include <dukeengine/Application.h>
#include <google/protobuf/descriptor.h>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace ::duke::protocol;
using namespace ::google::protobuf::serialize;

namespace { // empty namespace

struct SessionCreator {
    SessionCreator(QueueMessageIO& _io) :
        io(_io) {
    }
    ISession* create(boost::asio::io_service& service) {
        return new SocketSession(service, io.inputQueue, io.outputQueue);
    }
private:
    QueueMessageIO& io;
};

void launch(Session& s) {
    try {
        const SessionDescriptor & descriptor = s.descriptor();

        duke::protocol::Cache cache;
        cache.set_size(descriptor.cacheSize());
        cache.set_threading(descriptor.threadSize());
        cache.clear_region();
        int returnvalue = 0;
        Application(descriptor.rendererPath().c_str(), s.factory(), s.queue(), returnvalue, cache);
    } catch (std::exception & e) {
        std::cerr << "[Session::launch] Error: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[Session::launch] Unknown error." << std::endl;
    }
}

} // empty namespace


Session::Session() :
    mConnected(false) {
}

bool Session::start(void* handle) {
    try {
        if (connected())
            return false;

        mThread = boost::thread(&launch, boost::ref(*this));
        mConnected = true;

        // pop first msg (must be of type Renderer) to set the right handle
        if (handle) {
            SharedHolder holder;
            descriptor().getInitTimeQueue().tryPop(holder);
            if (isType< ::duke::protocol::Renderer> (*holder)) {
                ::duke::protocol::Renderer r = unpackTo< ::duke::protocol::Renderer> (*holder);
                r.set_handle((::google::protobuf::uint64) handle);
                push(mIo.inputQueue, r); // send to engine
            }
        }

        sendMsg(descriptor().getInitTimeQueue());

    } catch (std::exception& e) {
        std::cerr << "Error while connecting to server." << std::endl;
        return false;
    }
    return true;
}

bool Session::stop() {
    if (!connected())
        return false;
    quitRenderer(mIo.inputQueue);
    mIo.inputQueue.push(make_shared(quitSuccess()));
    mThread.join();
    return true;
}

bool Session::receiveMsg() {
    if (!connected())
        return false;
    SharedHolder holder;
    while (mIo.outputQueue.tryPop(holder)) {
        updateDescriptor(holder); // update session descriptor
        notify(holder); // notify observers
    }
    return false;
}

bool Session::sendMsg(MessageQueue & queue) {
    if (!connected())
        return false;
    SharedHolder holder;
    while (queue.tryPop(holder)) {
        notify(holder);
        mIo.inputQueue.push(holder);
    }
    return false;
}

// private
void Session::updateDescriptor(SharedHolder pholder) {
    const MessageHolder &holder = *pholder;
    const ::google::protobuf::Descriptor* pDescriptor = descriptorFor(holder);
    if (isType<Transport> (pDescriptor))
        analyseTransport(unpackTo<Transport> (holder));
    else if (isType<PlaybackState> (pDescriptor))
        analysePlaybackState(unpackTo<PlaybackState> (holder));
    else {
        // pass
    }
}

// private
void Session::analyseTransport(Transport transport) {
    switch (transport.type()) {
        case Transport_TransportType_PLAY:
        case Transport_TransportType_STOP:
        case Transport_TransportType_STORE:
        case Transport_TransportType_CUE_FIRST:
        case Transport_TransportType_CUE_LAST:
        case Transport_TransportType_CUE_STORED:
            break;
        case Transport_TransportType_CUE:
            if (transport.cue().cueclip())
                break;
            if (transport.cue().cuerelative())
                break;
            descriptor().setCurrentFrame(transport.cue().value());
            break;
    }
}

// private
void Session::analysePlaybackState(PlaybackState playbackstate) {
    if (playbackstate.has_frameratenumerator()) {
        descriptor().setFramerate(playbackstate.frameratenumerator());
    }
}

//// private - separate thread
//void Session::run() {
//    try {
//        using namespace boost::asio;
//        using namespace boost::asio::ip;
//        using google::protobuf::serialize::duke_server;
//        SessionCreator creator(mIo);
//        boost::asio::ip::tcp::resolver::query query(mIP, boost::lexical_cast<std::string>(mPort));
//        duke_client client(query, boost::bind(&SessionCreator::create, &creator, _1));
//        if (client.error())
//            return;
//        mConnected = true;
//        client.run(); // blocking
//    } catch (std::exception & e) {
//        std::cerr << "Error: " << e.what() << std::endl;
//    } catch (...) {
//        std::cerr << "Unknown error." << std::endl;
//    }
//    mConnected = false;
//}

#include "dkxSession.h"
#include <dukeapi/io/SocketMessageIO.h>
#include <dukeapi/core/messageBuilder/Commons.h>
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

} // empty namespace


Session::Session() :
    mFrame(0), mPlaying(false), mIP("127.0.0.1"), mPort(7171), mConnected(false) {
}

bool Session::load() {
    return false;
}

bool Session::save() {
    return false;
}

bool Session::startSession(const std::string& ip, short port, void* handle) {
    try {
        mIP = ip;
        mPort = port;
        if (connected())
            return false;
        mThread = boost::thread(&Session::run, this);
        boost::this_thread::sleep(boost::posix_time::millisec(40));

        // edit the renderer with the right handle
        Renderer & renderer = mDescriptor.renderer();
        renderer.set_handle((::google::protobuf::uint64) handle);

        MessageQueue q;
        // send the renderer
        push(q, renderer);
        sendMsg(q);
        // send prepared msg
        sendInitTimeMsg(mInitTimeMsgQueue);
        // send the playlist
        push(q, mDescriptor.playlist());
        sendMsg(q);

    } catch (std::exception& e) {
        std::cerr << "Error while connecting to server." << std::endl;
        return false;
    }
    return true;
}

bool Session::stopSession() {
    if (!connected())
        return false;
    quitRenderer(mIo.outputQueue);
    mIo.outputQueue.push(makeSharedHolder(quitSuccess()));
    mThread.join();
    return true;
}

bool Session::computeInMsg() {
    using namespace ::duke::protocol;
    SharedHolder holder;
    while (mIo.inputQueue.tryPop(holder)) {
        notify(holder);
        // check for Transport Msg
        if (::google::protobuf::serialize::isType<Transport>(*holder)) {
            const Transport t = ::google::protobuf::serialize::unpackTo<Transport>(*holder);
            switch (t.type()) {
                case Transport_TransportType_PLAY:
                case Transport_TransportType_STOP:
                case Transport_TransportType_STORE:
                case Transport_TransportType_CUE_FIRST:
                case Transport_TransportType_CUE_LAST:
                case Transport_TransportType_CUE_STORED:
                    break;
                case Transport_TransportType_CUE:
                    if (t.cue().cueclip())
                        break;
                    if (t.cue().cuerelative())
                        break;
                    mFrame = t.cue().value();
                    break;
            }
        }
    }
    return false;
}

bool Session::sendMsg(MessageQueue & queue) {
    SharedHolder holder;
    while (queue.tryPop(holder)) {
        notify(holder);
        mIo.outputQueue.push(holder);
    }
    return false;
}

bool Session::addInitTimeMsg(MessageQueue & queue) {
    SharedHolder holder;
    while (queue.tryPop(holder)) {
        mInitTimeMsgQueue.push(holder);
    }
    return true;
}

// private
bool Session::sendInitTimeMsg(MessageQueue & queue) {
    SharedHolder holder;
    while (queue.tryPop(holder)) {
        if (::google::protobuf::serialize::isType<Renderer>(*holder)) {
            continue;
        }
        notify(holder);
        mIo.outputQueue.push(holder);
    }
    return false;
}

// private - separate thread
void Session::run() {
    try {
        using namespace boost::asio;
        using namespace boost::asio::ip;
        using google::protobuf::serialize::duke_server;
        SessionCreator creator(mIo);
        boost::asio::ip::tcp::resolver::query query(mIP, boost::lexical_cast<std::string>(mPort));
        duke_client client(query, boost::bind(&SessionCreator::create, &creator, _1));
        if (client.error())
            return;
        mConnected = true;
        client.run(); // blocking
    } catch (std::exception & e) {
        std::cerr << "Error: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown error." << std::endl;
    }
    mConnected = false;
}

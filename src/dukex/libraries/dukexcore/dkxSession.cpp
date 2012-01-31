#include "dkxSession.h"
#include <dukeapi/io/SocketMessageIO.h>
#include <dukeapi/core/messageBuilder/Commons.h>
#include <boost/lexical_cast.hpp>

using namespace std;
//using namespace ::duke::protocol;
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
    mFrame(0), mPlaying(false), mIP("127.0.0.1"), mPort(7171), mFile(""), mConnected(false), mDirty(false) {
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

        startRenderer(mIo.outputQueue, true, 400, 300, handle);
        ::duke::protocol::Engine eOne;
        eOne.set_action(::duke::protocol::Engine_Action_RENDER_START);
        push(mIo.outputQueue, eOne);

    } catch (std::exception& e) {
        std::cerr << "Error while connecting to server." << std::endl;
        return false;
    }
    return true;
}

bool Session::stopSession() {
    if (!connected())
        return false;
    stopRenderer(mIo.outputQueue);
    mIo.outputQueue.push(makeSharedHolder(quitSuccess()));
    mThread.join();
    return true;
}

bool Session::computeInMsg() {
    SharedHolder holder;
    while (mIo.inputQueue.tryPop(holder)) {
        notify(holder);
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

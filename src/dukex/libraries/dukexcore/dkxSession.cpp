#include "dkxSession.h"
#include <dukeapi/SocketMessageIO.h>
#include <dukeapi/messageBuilder/Commons.h>
#include <dukeengine/Application.h>
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

void launch(int & returnvalue, const std::string & rendererpath, ImageDecoderFactoryImpl& decoder, QueueMessageIO & io, uint64_t cacheSize, size_t threads) {
    try {
        duke::protocol::Cache cache;
        cache.set_size(cacheSize);
        cache.set_threading(threads);
        cache.clear_region();
        Application(rendererpath.c_str(), decoder, io, returnvalue, cache);
    } catch (std::exception & e) {
        std::cerr << "[Session::launch] Error: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "[Session::launch] Unknown error." << std::endl;
    }
}

} // empty namespace


Session::Session() :
    mFrame(0), mPlaying(false), mIP("127.0.0.1"), mPort(7171), mConnected(false) {
}

bool Session::startSession(void* handle) {
    try {
        if (connected())
            return false;

//        // CLIENT MODE
//        mThread = boost::thread(&Session::run, this);
//        boost::this_thread::sleep(boost::posix_time::millisec(40));

        int returnvalue = 0;
        mThread = boost::thread(&launch, returnvalue, mRendererPath, boost::ref(mImageDecoderFactory), boost::ref(mIo), mCacheSize, mThreadSize);
//        boost::this_thread::sleep(boost::posix_time::millisec(40));
        mConnected = true;

        // edit the renderer with the right handle
        ::duke::protocol::Renderer & renderer = mDescriptor.renderer();
        renderer.set_handle((::google::protobuf::uint64) handle);

        MessageQueue q;

        // send the renderer
        push(q, renderer);
        sendMsg(q);

        // after the renderer, send all init-time msgs
        sendMsg(mInitTimeMsgQueue);


    } catch (std::exception& e) {
        std::cerr << "Error while connecting to server." << std::endl;
        return false;
    }
    return true;
}

bool Session::stopSession() {
    if (!connected())
        return false;

//    // CLIENT MODE
//    quitRenderer(mIo.outputQueue);
//    mIo.outputQueue.push(makeSharedHolder(quitSuccess()));
//    mThread.join();

    quitRenderer(mIo.inputQueue);
    mIo.inputQueue.push(make_shared(quitSuccess()));
    mThread.join();
    return true;
}

bool Session::computeInMsg() {
    using namespace ::duke::protocol;
    SharedHolder holder;
    while (mIo.outputQueue.tryPop(holder)) {
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
        mIo.inputQueue.push(holder);
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

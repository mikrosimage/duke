#ifndef DKXSESSION_H
#define DKXSESSION_H

#include "dkxObservable.h"
#include "dkxSessionDescriptor.h"
#include <dukeapi/QueueMessageIO.h>
#include <dukeengine/host/io/ImageDecoderFactoryImpl.h>
#include <boost/thread.hpp>

// forward declaration
class NodeManager;

class Session : public Observable {

public:
    typedef boost::shared_ptr<Session> ptr;

public:
    Session();

public:
    bool startSession(void* handle = NULL);
    bool stopSession();
    bool computeInMsg();
    bool sendMsg(MessageQueue & queue);

public:
    inline const size_t frame() const {
        return mFrame;
    }
    inline const bool isPlaying() const {
        return mPlaying;
    }
    inline void setIsPlaying(const bool b) {
        mPlaying = b;
    }
    inline const std::string & ip() const {
        return mIP;
    }
    inline void setIp(const std::string ip) {
        mIP = ip;
    }
    inline const short port() const {
        return mPort;
    }
    inline void setPort(const short port) {
        mPort = port;
    }
    inline const uint64_t cacheSize() const {
        return mCacheSize;
    }
    inline void setCacheSize(const uint64_t size) {
        mCacheSize = size;
    }
    inline const size_t threadSize() const {
        return mThreadSize;
    }
    inline void setThreadSize(const size_t size) {
        mThreadSize = size;
    }
    inline const bool connected() const {
        return mConnected;
    }
    inline SessionDescriptor& descriptor() {
        return mDescriptor;
    }
    inline const SessionDescriptor& descriptor() const {
        return mDescriptor;
    }
    inline void setDescriptor(const SessionDescriptor& descriptor) {
        mDescriptor = descriptor;
    }
    inline MessageQueue & getInitTimeMsgQueue() {
        return mInitTimeMsgQueue;
    }
    inline void setRendererPath(const std::string& path) {
        mRendererPath = path;
    }
    inline const char ** getAvailableExtensions() const {
        return mImageDecoderFactory.getAvailableExtensions();
    }


private:
    Session(const Session&);
    const Session& operator=(const Session&);

private:
    void run();

private:
    size_t mFrame;
    bool mPlaying;
    std::string mIP;
    short mPort;
    uint64_t mCacheSize;
    size_t mThreadSize;
    bool mConnected;
    std::string mRendererPath;
    ImageDecoderFactoryImpl mImageDecoderFactory;
    SessionDescriptor mDescriptor;

private:
    QueueMessageIO mIo;
    MessageQueue mInitTimeMsgQueue;
    boost::thread mThread;
};

#endif // DKXSESSION_H

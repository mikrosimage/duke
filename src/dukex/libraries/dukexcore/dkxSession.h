#ifndef DKXSESSION_H
#define DKXSESSION_H

#include "dkxObservable.h"
#include "dkxSessionDescriptor.h"
#include <dukeapi/io/QueueMessageIO.h>
#include <boost/thread.hpp>

// forward declaration
class NodeManager;

class Session : public Observable {

public:
    typedef boost::shared_ptr<Session> ptr;

public:
    Session();

public:
    bool load();
    bool save();
    bool startSession(const std::string& ip, short port, void* handle = NULL);
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
    inline const short port() const {
        return mPort;
    }
    inline const std::string & file() const {
        return mFile;
    }
    inline const bool connected() const {
        return mConnected;
    }
    inline const bool dirty() const {
        return mDirty;
    }
    inline SessionDescriptor& descriptor() {
        return mDescriptor;
    }

private:
    Session(const Session&);
    const Session& operator=(const Session&);
    void run();

private:
    size_t mFrame;
    bool mPlaying;
    std::string mIP;
    short mPort;
    std::string mFile;
    bool mConnected;
    bool mDirty;
    SessionDescriptor mDescriptor;

private:
    QueueMessageIO mIo;
    boost::thread mThread;
};

#endif // DKXSESSION_H

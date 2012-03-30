#ifndef DKXSESSION_H
#define DKXSESSION_H

#include "dkxObservable.h"
#include "dkxSessionDescriptor.h"
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
    bool start(void* handle = NULL);
    bool stop();
    bool receiveMsg();
    bool sendMsg(MessageQueue & queue);

public:
    inline const bool connected() const {
        return mConnected;
    }
    inline SessionDescriptor& descriptor() {
        return mDescriptor;
    }
    inline const SessionDescriptor& descriptor() const {
        return mDescriptor;
    }
    inline ImageDecoderFactoryImpl & factory() {
        return mImageDecoderFactory;
    }
    inline const ImageDecoderFactoryImpl & factory() const {
        return mImageDecoderFactory;
    }
    inline QueueMessageIO & queue() {
        return mIo;
    }
    inline const QueueMessageIO & queue() const {
        return mIo;
    }

private:
    Session(const Session&);
    const Session& operator=(const Session&);

private:
    void updateDescriptor(::google::protobuf::serialize::SharedHolder);
    void analyseTransport(::duke::protocol::Transport transport);
    void analysePlaybackState(::duke::protocol::PlaybackState playback);

private:
    bool mConnected;
    ImageDecoderFactoryImpl mImageDecoderFactory;
    SessionDescriptor mDescriptor;
    QueueMessageIO mIo;
    boost::thread mThread;
};

#endif // DKXSESSION_H

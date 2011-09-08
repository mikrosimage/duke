#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <SFML/Audio.hpp>
#include <communication.pb.h>
#include "ISyncMethod.h"

class AudioEngine {

    enum AUDIOENGINE_STATUS {
        AE_RESETED = 0, AE_READY, AE_ERROR
    };

public:
    typedef boost::shared_ptr<AudioEngine> ptr;
    typedef boost::function<size_t()> CurrentVideoFrameCallback;
    AudioEngine(CurrentVideoFrameCallback f, const float & _videoFramerate = 25.f);
    virtual ~AudioEngine();

public:
    bool load(const ::protocol::duke::Playlist & _playlist);
    bool load(const std::string & _file);
    void play();
    void pause();
    void stop();
    void rewind();
    void applyTransport(const ::protocol::duke::Transport& transport);

private:
    void instantSync();
    void constantSyncLoop();

private:
    sf::Music m_Music;
    boost::thread m_SyncThread;
    ISyncMethod::ptr m_SyncMethod;
    CurrentVideoFrameCallback m_fCurrentVideoFrame;
    float m_videoFramerate;
    bool m_bIsSyncing;
    AUDIOENGINE_STATUS m_Status;
};

#endif // AUDIOENGINE_H

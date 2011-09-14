#include "AudioEngine.h"
#include "InstantSyncMethod.h"
#include "AverageSyncMethod.h"
#include <stdio.h>
#include <stdlib.h>
#include <cmath>

using namespace ::duke::protocol;
using namespace ::std;

#define _MAX_PITCH_ 2.f
#define _MIN_PITCH_ 0.5f

#ifdef DEBUG
#define _DEBUG_CHECK_STATUS(action) {\
    switch(m_Status){\
    case AE_RESETED:\
        cerr << "[Sound] " << action << ": Nothing to do (no input file specified)" << endl; break;\
    case AE_ERROR:\
        cerr << "[Sound] " << action << ": Error while loading audio source file" << endl; break;\
    default: \
        cerr << "[Sound] " << action << endl; break;\
    } \
    }
#else
#define _DEBUG_CHECK_STATUS(action) {}
#endif


AudioEngine::AudioEngine(AudioEngine::CurrentVideoFrameCallback _f, const float & _videoFramerate) :
    m_fCurrentVideoFrame(_f), m_videoFramerate(_videoFramerate), m_bIsSyncing(false), m_Status(AE_RESETED) {
    m_SyncMethod = AverageSyncMethod::ptr(new AverageSyncMethod());
}

AudioEngine::~AudioEngine() {
    stop();
}

bool AudioEngine::load(const Playlist & _playlist) {
    if (!_playlist.has_audiosource()) {
        m_Status = AE_ERROR;
        _DEBUG_CHECK_STATUS ("Load")
        return false;
    }
    AudioSource source = _playlist.audiosource();
    if (!source.has_name()) {
        m_Status = AE_ERROR;
        _DEBUG_CHECK_STATUS("Load")
        return false;
    }
    return load(source.name());
}

bool AudioEngine::load(const string & _file) {
    stop();
    if (!m_Music.OpenFromFile(_file)) {
        m_Status = AE_ERROR;
        _DEBUG_CHECK_STATUS("Load")
        return false;
    }
    m_Status = AE_READY;
    return true;
}

void AudioEngine::play() {
    _DEBUG_CHECK_STATUS("Play")
    if (m_Status != AE_READY)
        return;
    instantSync();
    m_Music.Play();
    if (!m_bIsSyncing) {
        m_bIsSyncing = true;
        m_SyncThread = boost::thread(&AudioEngine::constantSyncLoop, this);
    }
}

void AudioEngine::pause() {
    _DEBUG_CHECK_STATUS("Pause")
    m_Music.Pause();
    m_bIsSyncing = false;
    m_SyncThread.join();
}

void AudioEngine::stop() {
    _DEBUG_CHECK_STATUS("Stop")
    m_Music.Stop();
    m_bIsSyncing = false;
    m_SyncThread.join();
}

void AudioEngine::rewind() {
    _DEBUG_CHECK_STATUS("Rewind")
    m_Music.SetPlayingOffset(0.f);
}

void AudioEngine::applyTransport(const Transport& transport){
    switch (transport.type()) {
        case Transport_TransportType_PLAY:
            play();
            break;
        case Transport_TransportType_STOP:
        case Transport_TransportType_CUE_FIRST:
        case Transport_TransportType_CUE:
        case Transport_TransportType_CUE_STORED:
        case Transport_TransportType_CUE_LAST:
        case Transport_TransportType_STORE:
            pause();
            break;
        default:
            break;
    }
}

// private
void AudioEngine::instantSync() {
    float videoOffset = (m_fCurrentVideoFrame() / m_videoFramerate) * 1000.f;
    m_Music.SetPlayingOffset(videoOffset);
}

// private
void AudioEngine::constantSyncLoop() {
    while (m_bIsSyncing) {
        float videoOffset = m_fCurrentVideoFrame() / m_videoFramerate;
        float soundOffset = m_Music.GetPlayingOffset() / 1000.f;
        float currentPitch = 1.f;
        if (m_SyncMethod.get() != NULL) {
            bool computeEnded = false;
            m_SyncMethod->init(videoOffset, soundOffset, m_videoFramerate);
            do {
                computeEnded = m_SyncMethod->compute(currentPitch);
                if (currentPitch <= _MIN_PITCH_ || currentPitch >= _MAX_PITCH_) {
                    instantSync();
                }
                m_Music.SetPitch(currentPitch);
                m_SyncMethod->wait();
            } while (!computeEnded);
        }
    }
}

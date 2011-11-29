#include "AudioEngine.h"
#include <cmath>

using namespace ::duke::protocol;
using namespace ::std;
#define MAXMSOFFSET 100

namespace {
    const string HEADER = "[Audio] ";
} // private namespace


AudioEngine::AudioEngine() :
    bFileLoaded(false) {
    m_Music.SetVolume(50);
}

AudioEngine::~AudioEngine() {
    stop();
}

bool AudioEngine::load(const Playlist & _playlist) {
    if (!_playlist.has_audiosource())
        return false;
    AudioSource source = _playlist.audiosource();
    if (!source.has_name())
        return false;
    return load(source.name());
}

bool AudioEngine::load(const string & _file) {
    cout << HEADER << "Loading " << _file << endl;
    bFileLoaded = m_Music.OpenFromFile(_file);
    return bFileLoaded;
}

void AudioEngine::play() {
    if (bFileLoaded)
        m_Music.Play();
}

void AudioEngine::pause() {
    m_Music.Pause();
}

void AudioEngine::stop() {
    m_Music.Stop();
}

void AudioEngine::checksync(::boost::chrono::high_resolution_clock::duration d) {
    if (!bFileLoaded)
        return;
    float soundOffset = m_Music.GetPlayingOffset();
    float imageOffset = d.count() / 1000000.f;
    if (fabs(imageOffset - soundOffset) > MAXMSOFFSET) {
        sync(d);
#if defined DEBUG
        cout << HEADER << "Syncing" << endl;
#endif
    }
}

void AudioEngine::sync(::boost::chrono::high_resolution_clock::duration d) {
    if (!bFileLoaded)
        return;
    float imageOffset = d.count() / 1000000.f;
    m_Music.SetPlayingOffset(imageOffset);
}

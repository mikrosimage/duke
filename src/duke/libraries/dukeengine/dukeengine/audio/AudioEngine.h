#ifndef AUDIOENGINE_H
#define AUDIOENGINE_H

#include <SFML/Audio.hpp>
#include <boost/chrono.hpp>
#include <player.pb.h>

namespace duke {namespace protocol {class Playlist;}}

class AudioEngine {

public:
    AudioEngine();
    ~AudioEngine();

public:
    bool load(const ::duke::protocol::Scene & scene);
    bool load(const std::string & file);
    void play();
    void pause();
    void stop();
    void checksync(::boost::chrono::high_resolution_clock::duration d);
    void sync(::boost::chrono::high_resolution_clock::duration d);

private:
    sf::Music m_Music;
    bool bFileLoaded;
};

#endif // AUDIOENGINE_H

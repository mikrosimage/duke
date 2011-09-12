#ifndef PLAYBACKREADER_H_
#define PLAYBACKREADER_H_

#include <dukeapi/core/IMessageIO.h>
#include <fstream>
#include <memory>

class PlaybackReader : public IMessageIO {
public:
    PlaybackReader(const char* filename);
    virtual ~PlaybackReader();

    virtual void push(const SharedMessage& holder);
    virtual void waitPop(SharedMessage& holder);
    virtual bool tryPop(SharedMessage& holder);

private:
    std::ifstream m_Stream;
};

#endif /* PLAYBACKREADER_H_ */

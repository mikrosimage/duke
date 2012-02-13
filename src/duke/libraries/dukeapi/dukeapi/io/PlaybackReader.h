#ifndef PLAYBACKREADER_H_
#define PLAYBACKREADER_H_

#include <dukeapi/IMessageIO.h>
#include <fstream>
#include <memory>

class PlaybackReader : public IMessageIO {
public:
    PlaybackReader(const char* filename);
    virtual ~PlaybackReader();

    virtual void push(const ::google::protobuf::serialize::SharedHolder& holder);
    virtual void waitPop(::google::protobuf::serialize::SharedHolder& holder);
    virtual bool tryPop(::google::protobuf::serialize::SharedHolder& holder);

private:
    std::ifstream m_Stream;
};

#endif /* PLAYBACKREADER_H_ */

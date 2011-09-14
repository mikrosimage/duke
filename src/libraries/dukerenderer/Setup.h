#ifndef SETUP_H_
#define SETUP_H_

#include <dukeio/ImageDescription.h>
#include <dukeapi/protocol/player/protocol.pb.h>
#include <vector>

typedef struct Setup {
    size_t m_iFrame;
    std::vector<duke::protocol::Clip> m_Clips;
    std::vector<ImageDescription> m_Images;
} Setup;

#endif /* SETUP_H_ */

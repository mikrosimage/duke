/*
 * ClipHelper.h
 *
 *  Created on: 14 avr. 2010
 *      Author: Guillaume Chatelet
 */

#ifndef CLIPHELPER_H_
#define CLIPHELPER_H_

#include <dukeapi/protocol/player/communication.pb.h>
#include <boost/filesystem.hpp>

size_t duration(const size_t in, const size_t out);
bool existAt(const size_t in, const size_t out, const size_t frame);
bool isRecFrame(const ::protocol::duke::Clip& clip, const size_t frame);
bool isSrcFrame(const ::protocol::duke::Clip& clip, const size_t frame);

class ClipHelper {
private:
    const ::protocol::duke::Clip m_Clip;
    const size_t m_uStartIndex;
    const size_t m_uEndIndex;
    const size_t m_uPatternSize;
    const uint32_t m_uClipHash;

public:
    ClipHelper(const ::protocol::duke::Clip& clip);
    ~ClipHelper();

    ::boost::filesystem::path getFilenameAtFrame(const size_t frame) const;
    ::boost::filesystem::path getFilenameAt(const size_t frame) const;
    uint64_t getHashAt(const size_t frame) const;
    uint32_t getClipHash() const {
        return m_uClipHash;
    }
    const ::protocol::duke::Clip& getClip() const { return m_Clip; }
    bool isRecFrame(const size_t frame) const;
    bool isSrcFrame(const size_t frame) const;
    size_t getSourceFrame(const size_t recordFrame) const;
};

#endif /* CLIPHELPER_H_ */

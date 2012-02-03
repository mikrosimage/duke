#ifndef CLIPHELPER_H_
#define CLIPHELPER_H_

#include <player.pb.h>
#include <boost/filesystem.hpp>

bool existAt(const uint32_t in, const uint32_t out, const uint32_t frame);
bool isRecFrame(const ::duke::protocol::Clip& clip, const uint32_t frame);
bool isSrcFrame(const ::duke::protocol::Clip& clip, const uint32_t frame);

uint32_t duration(const uint32_t in, const uint32_t out);
uint32_t clampFrame(uint32_t first, uint32_t last, uint32_t current);
uint32_t offsetLoopFrame(uint32_t first, uint32_t last, uint32_t current, int32_t offset);
uint32_t offsetClampFrame(uint32_t first, uint32_t last, uint32_t current, int32_t offset);

class ClipHelper {
private:
    const ::duke::protocol::Clip m_Clip;
    const size_t m_uStartIndex;
    const size_t m_uEndIndex;
    const size_t m_uPatternSize;
    const uint32_t m_uClipHash;

public:
    ClipHelper(const ::duke::protocol::Clip& clip);
    ~ClipHelper();

    ::boost::filesystem::path getFilenameAtFrame(const uint32_t frame) const;
    ::boost::filesystem::path getFilenameAt(const uint32_t frame) const;
    uint64_t getHashAt(const uint32_t frame) const;
    uint32_t getClipHash() const {
        return m_uClipHash;
    }
    const ::duke::protocol::Clip& getClip() const { return m_Clip; }
    bool isRecFrame(const uint32_t frame) const;
    bool isSrcFrame(const uint32_t frame) const;
    uint32_t getSourceFrame(const uint32_t recordFrame) const;
};

#endif /* CLIPHELPER_H_ */

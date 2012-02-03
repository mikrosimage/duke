#include "ClipHelper.h"
#include <dukeengine/utils/Rational.h>

#include <boost/functional/hash.hpp>

#include <string>
#include <algorithm>

using namespace std;
using namespace boost::filesystem;

bool existAt(const uint32_t in, const uint32_t out, const uint32_t frame) {
    assert( out >= in);
    return frame >= in && frame < out;
}

bool isRecFrame(const ::duke::protocol::Clip& clip, const uint32_t frame) {
    return ::existAt(clip.recin(), clip.recout(), frame);
}

bool isSrcFrame(const ::duke::protocol::Clip& clip, const uint32_t frame) {
    return ::existAt(clip.srcin(), clip.srcout(), frame);
}

uint32_t duration(const uint32_t in, const uint32_t out) {
    assert(out >= in);
    return out - in;
}

uint32_t clampFrame(uint32_t first, uint32_t last, uint32_t current) {
    assert(last>=first);
    return max(min(current, last), first);
}

static inline uint32_t frameDuration(uint32_t first, uint32_t last) {
    assert(first<=last);
    assert(last-first!=uint32_t(-1));
    return last - first + 1;
}

static inline uint32_t reducedOffset(uint32_t first, uint32_t last, int32_t offset) {
    const bool forward = offset > 0;
    const uint32_t positiveOffset = forward ? offset : -offset;
    return positiveOffset % frameDuration(first, last);
}

template<bool isCycling>
static inline uint32_t offsetFrame(uint32_t first, uint32_t last, uint32_t current, int32_t offset) {
    assert(first<=current);
    assert(current<=last);
    if (offset == 0)
        return current;
    // offset frame is within [first,last]
    const bool forward = offset > 0;
    const uint32_t positiveOffset = forward ? offset : -offset;
    const uint32_t distanceToLast = frameDuration(current, last);
    const uint32_t distanceToFirst = frameDuration(first, current);
    const uint32_t semiRangeDuration = forward ? distanceToLast : distanceToFirst;
    // inside
    if (positiveOffset < semiRangeDuration)
        return forward ? current + positiveOffset : current - positiveOffset;
    const int32_t reduced = reducedOffset(first, last, offset);
    if (forward)
        return isCycling ? first + reduced - distanceToLast : last;
    else
        return isCycling ? last - reduced + distanceToFirst : first;
}

uint32_t offsetClampFrame(uint32_t first, uint32_t last, uint32_t current, int32_t offset) {
    return offsetFrame<false>(first, last, current, offset);
}

uint32_t offsetLoopFrame(uint32_t first, uint32_t last, uint32_t current, int32_t offset) {
    return offsetFrame<true>(first, last, current, offset);
}

ClipHelper::ClipHelper(const ::duke::protocol::Clip& clip) :
    m_Clip(clip), //
    m_uStartIndex(m_Clip.filename().find_first_of('#')), //
    m_uEndIndex(m_Clip.filename().find_last_of('#')), //
    m_uPatternSize(m_uEndIndex - m_uStartIndex + 1), //
    m_uClipHash(boost::hash_value(m_Clip.path() + m_Clip.filename())) {
    if (m_Clip.recout() <= m_Clip.recin())
        throw std::logic_error("clip recout is less or equal than recin");
}

ClipHelper::~ClipHelper() {
}

bool ClipHelper::isRecFrame(const uint32_t frame) const {
    return ::isRecFrame(m_Clip, frame);
}

bool ClipHelper::isSrcFrame(const uint32_t frame) const {
    return ::isSrcFrame(m_Clip, frame);
}

::boost::filesystem::path ClipHelper::getFilenameAt(const uint32_t frame) const {
    return getFilenameAtFrame(getSourceFrame(frame));
}

uint64_t ClipHelper::getHashAt(const uint32_t frame) const {
    uint64_t hash = m_uClipHash;
    hash <<= 32;
    hash += getSourceFrame(frame);
    return hash;
}

uint32_t ClipHelper::getSourceFrame(const uint32_t recordFrame) const {
    if (!m_Clip.has_srcout())
        return recordFrame - m_Clip.recin() + m_Clip.srcin();
    cerr << "slow motion and reverse is not yet implemented" << endl;
    assert(false);
    return m_Clip.recin();

//    const uint32_t frameIn(m_Clip.srcin());
//    const uint32_t frameOut(m_Clip.srcout());
//    const bool forward = frameOut >= frameIn;
//    const uint32_t srcDuration = forward ? ::duration(frameIn, frameOut) : ::duration(frameOut, frameIn);
//    if (srcDuration == 0)
//        return frameIn;
//    const uint32_t recIn(m_Clip.recin());
//    const uint32_t recOut(m_Clip.recout());
//    const uint32_t recDuration = ::duration(recIn, recOut);
//    const Rational speed(Rational(srcDuration) / Rational(recDuration));
//    const uint32_t framesInSource(::boost::rational_cast<uint32_t>(speed * (recordFrame - m_Clip.recin())));
//    if (forward)
//        return frameIn + framesInSource;
//    return frameIn - framesInSource;
}

#include <sstream>
::boost::filesystem::path ClipHelper::getFilenameAtFrame(const uint32_t frame) const {
    ::boost::filesystem::path intoPath;
    if (m_Clip.filename().empty())
        return intoPath;

    ostringstream frameStringStream;

    frameStringStream.width(m_uPatternSize);
    frameStringStream.fill('0');
    frameStringStream.flags(::ios::right);
    frameStringStream << frame;
    const string frameString(frameStringStream.str());
    if (frameString.size() > m_uPatternSize)
        throw std::logic_error(std::string("Requested frame ") + frameString + " cannot fit in pattern " + m_Clip.filename());

    string filename(m_Clip.filename());

    copy(frameString.begin(), frameString.end(), filename.begin() + m_uStartIndex);

    intoPath = m_Clip.path();
    intoPath /= filename;
    return intoPath;
}


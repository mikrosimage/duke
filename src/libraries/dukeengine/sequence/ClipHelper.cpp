/*
 * ClipHelper.cpp
 *
 *  Created on: 14 avr. 2010
 *      Author: Guillaume Chatelet
 */

#include "ClipHelper.h"
#include <utils/Rational.h>

#include <boost/functional/hash.hpp>

#include <string>
#include <algorithm>

using namespace std;
using namespace boost::filesystem;

bool existAt(const size_t in, const size_t out, const size_t frame) {
    return frame >= in && frame < out;
}

bool isRecFrame(const ::protocol::duke::Clip& clip, const size_t frame) {
    return ::existAt(clip.recin(), clip.recout(), frame);
}

bool isSrcFrame(const ::protocol::duke::Clip& clip, const size_t frame) {
    return ::existAt(clip.srcin(), clip.srcout(), frame);
}

size_t duration(const size_t in, const size_t out) {
    assert( out >= in );
    return (out - 1) - in;
}

ClipHelper::ClipHelper(const ::protocol::duke::Clip& clip) :
    m_Clip(clip),//
            m_uStartIndex(m_Clip.filename().find_first_of('#')), //
            m_uEndIndex(m_Clip.filename().find_last_of('#')), //
            m_uPatternSize(m_uEndIndex - m_uStartIndex + 1), //
            m_uClipHash(boost::hash_value(m_Clip.path()+m_Clip.filename())) {
    if (!m_Clip.filename().empty())
        if (m_uStartIndex == string::npos || m_uEndIndex == string::npos)
            throw std::logic_error(string("Invalid pattern for filename ") + m_Clip.filename());
    if (m_Clip.recout() <= m_Clip.recin())
        throw std::logic_error("clip recout is less or equal than recin");
}

ClipHelper::~ClipHelper() {
}

bool ClipHelper::isRecFrame(const size_t frame) const {
    return ::isRecFrame(m_Clip, frame);
}

bool ClipHelper::isSrcFrame(const size_t frame) const {
    return ::isSrcFrame(m_Clip, frame);
}

::boost::filesystem::path ClipHelper::getFilenameAt(const size_t frame) const {
    return getFilenameAtFrame(getSourceFrame(frame));
}

uint64_t ClipHelper::getHashAt(const size_t frame) const {
    uint64_t hash = m_uClipHash ;
    hash <<= 32;
    hash += getSourceFrame(frame);
    return hash;
}

size_t ClipHelper::getSourceFrame(const size_t recordFrame) const {
    if (!m_Clip.has_srcout())
        return recordFrame - m_Clip.recin() + m_Clip.srcin();
    const size_t srcIn(m_Clip.srcin());
    const size_t srcOut(m_Clip.srcout());
    const bool forward = srcOut >= srcIn;
    const size_t srcDuration = forward ? ::duration(srcIn, srcOut) : ::duration(srcOut, srcIn);
    if (srcDuration == 0)
        return srcIn;
    const size_t recIn(m_Clip.recin());
    const size_t recOut(m_Clip.recout());
    const size_t recDuration = ::duration(recIn, recOut);
    const Rational speed(Rational(srcDuration) / Rational(recDuration));
    const size_t framesInSource(::boost::rational_cast<size_t>(speed * (recordFrame - m_Clip.recin())));
    if (forward)
        return srcIn + framesInSource;
    return srcIn - framesInSource - 1;
}

#include <sstream>
::boost::filesystem::path ClipHelper::getFilenameAtFrame(const size_t frame) const {
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


#ifndef PLAYBACKUTILS_H_
#define PLAYBACKUTILS_H_

#include <sequence/Range.h>
#include <dukeapi/sequence/PlaylistHelper.h>

namespace sequence {

enum ERangeIteratorStrategy {
    FORWARD, BALANCE, REVERSE
};

struct RangeIterator {
    RangeIterator(const Range &withinRange, bool isCycling, unsigned int startAt, ERangeIteratorStrategy strategy);
    unsigned int next();
private:
    Range range;
    unsigned int initialPosition;
    bool isCycling;
    ERangeIteratorStrategy strategy;
    unsigned int index;
};

RangeIterator fromPlaylist(const duke::protocol::PlaylistHelper &helper, unsigned int startAt, ERangeIteratorStrategy strategy){
    return RangeIterator(helper.range, helper.playlist.loop(), startAt, strategy);
}

} // namespace sequence
#endif /* PLAYBACKUTILS_H_ */

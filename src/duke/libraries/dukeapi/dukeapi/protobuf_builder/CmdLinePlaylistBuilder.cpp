/*
 * CmdLinePlaylistBuilder.cpp
 *
 *  Created on: 22 fevr. 2012
 *      Author: Guillaume Chatelet
 */

#include "CmdLinePlaylistBuilder.h"

#include <sequence/BrowseItem.h>
#include <sequence/DisplayUtils.h>
#include <sequence/parser/Browser.h>

#include <dukeapi/protobuf_builder/PlaylistBuilder.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <set>

#include <algorithm>

using namespace std;
using namespace boost::algorithm;
using namespace boost::filesystem;
using namespace duke::protocol;

using sequence::BrowseItem;

typedef vector<BrowseItem> BrowseItems;

namespace details {

struct ci_less : std::binary_function<std::string, std::string, bool> {
    inline bool operator()(const std::string & s1, const std::string & s2) const {
        return boost::algorithm::ilexicographical_compare(s1, s2);
    }
};

} // namespace details

static string g_ppl2(".ppl2");
static string g_ppl(".ppl");

struct CmdLinePlaylistBuilder::Pimpl : private boost::noncopyable {
    Pimpl(bool useContainingSequence, const char **validExtensions) :
                    useContainingSequence(useContainingSequence), trackBuilder(playlistBuilder.addTrack("default")) {
        for (; validExtensions != NULL && *validExtensions != NULL; ++validExtensions) {
            string extension = *validExtensions;
            if (!extension.empty() && extension[0] != '.')
                extension.insert(extension.begin(), '.');
            extensions.insert(extension);
        }
    }

    void ingest(BrowseItems items) {
        sequence::filterOut(items, boost::bind(&Pimpl::isInvalid, this, _1));
        for_each(items.begin(), items.end(), boost::bind(&TrackBuilder::addBrowseItem, boost::ref(trackBuilder), _1));
    }

    const bool useContainingSequence;
    PlaylistBuilder playlistBuilder;
    TrackBuilder trackBuilder;
private:
    bool isInvalid(const BrowseItem &item) const {
        return extensions.find(item.extension()) == extensions.end();
    }
    set<string, details::ci_less> extensions;
};

static inline void parsePPL2(const CmdLinePlaylistBuilder::Pimpl &pimpl, const path &filename) {
}

static inline void parsePPL(const CmdLinePlaylistBuilder::Pimpl &pimpl, const path &filename) {
}

static inline void parsePattern(const CmdLinePlaylistBuilder::Pimpl &pimpl, const path &filename) {
}

static inline void parseDirectory(CmdLinePlaylistBuilder::Pimpl &pimpl, const path &directory) {
    pimpl.ingest(sequence::parser::browse(directory.string().c_str(), false));
}

static inline bool contained(const string& filename, const BrowseItem & item) {
    return item.type == sequence::SEQUENCE && item.sequence.pattern.match(filename);
}

static inline bool notContained(const string& filename, const BrowseItem & item) {
    return !contained(filename, item);
}

static inline void parseFilename(CmdLinePlaylistBuilder::Pimpl &pimpl, const path &filename) {
    BrowseItems items;
    if (pimpl.useContainingSequence) {
        items = sequence::parser::browse(filename.parent_path().string().c_str(), false);
        sequence::filterOut(items, bind(&notContained, filename.filename().string(), _1));
    } else {
        items.push_back(sequence::create_file(filename));
    }
    pimpl.ingest(items);
}

static inline bool isPattern(const string &entry) {
    return entry.find_first_of("#@") != string::npos;
}

CmdLinePlaylistBuilder::CmdLinePlaylistBuilder(bool useContainingSequence, const char **validExtensions) :
                m_Pimpl(new Pimpl(useContainingSequence, validExtensions)) {
}

duke::protocol::Playlist CmdLinePlaylistBuilder::getPlaylist() const {
    return m_Pimpl->playlistBuilder;
}

void CmdLinePlaylistBuilder::process(const string& entry) {
    Pimpl &pimpl = *m_Pimpl.get();
    const path filename(entry);
    if (isPattern(entry)) {
        parsePattern(pimpl, filename);
        return;
    }
    // it is either a Playlist, a filename or a directory, it therefore must exists
    if (!exists(filename))
        throw ios_base::failure(string("Unable to open ") + filename.string());
    if (iends_with(entry, g_ppl2))
        parsePPL2(pimpl, filename);
    else if (iends_with(entry, g_ppl))
        parsePPL(pimpl, filename);
    else if (is_directory(filename))
        parseDirectory(pimpl, filename);
    else
        parseFilename(pimpl, filename);
}

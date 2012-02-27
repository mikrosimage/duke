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

#include <dukeapi/IMessageIO.h>
#include <dukeapi/protobuf_builder/PlaylistBuilder.h>
#include <dukeapi/protobuf_builder/ShaderBuilder.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>

#include <iostream>
#include <sstream>
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
static string HEADER = "[Playlist] ";

struct CmdLinePlaylistBuilder::Pimpl : private boost::noncopyable {
    Pimpl(IOQueueInserter &inserter, bool useContainingSequence, const char **validExtensions) :
                    inserter(inserter), useContainingSequence(useContainingSequence), trackBuilder(playlistBuilder.addTrack("default")), clipIndex(0) {
        for (; validExtensions != NULL && *validExtensions != NULL; ++validExtensions) {
            string extension = *validExtensions;
            if (!extension.empty() && extension[0] != '.')
                extension.insert(extension.begin(), '.');
            extensions.insert(extension);
        }
        shader_builder::pushCommonMessages(inserter);
    }

    void ingest(BrowseItems items) {
        sequence::filterOut(items, boost::bind(&Pimpl::isInvalid, this, _1));
        for (BrowseItems::iterator itr = items.begin(); itr != items.end(); ++itr) {
            cout << HEADER << *itr << endl;
            Clip &clip = trackBuilder.addBrowseItem(*itr);
            ostringstream name;
            name << "clip_" << clipIndex++;
            clip.set_name(name.str());
            shader_builder::adapt(inserter, clip, itr->extension());
        }
    }

    Playlist finalize() {
        return playlistBuilder;
    }

    IOQueueInserter &inserter;
    const bool useContainingSequence;
    PlaylistBuilder playlistBuilder;
    TrackBuilder trackBuilder;
    bool isInvalid(const BrowseItem &item) const {
        return extensions.find(item.extension()) == extensions.end();
    }
    set<string, details::ci_less> extensions;
    size_t clipIndex;
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

CmdLinePlaylistBuilder::CmdLinePlaylistBuilder(IOQueueInserter &inserter, bool useContainingSequence, const char **validExtensions) :
                m_Pimpl(new Pimpl(inserter, useContainingSequence, validExtensions)) {
}

Playlist CmdLinePlaylistBuilder::finalize() {
    return m_Pimpl->finalize();
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

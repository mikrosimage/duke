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
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

#include <fstream>
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
        transport.set_type(Transport_TransportType_CUE);
        transport.mutable_cue()->set_value(0);
    }

    void ingest(const BrowseItem &item, unsigned record = UINT_MAX) {
        if (isInvalid(item))
            return;
        Clip &clip = trackBuilder.addBrowseItem(item, record);
        ostringstream name;
        name << "clip_" << clipIndex++;
        clip.set_name(name.str());
        shader_builder::adapt(inserter, clip, item.extension());
        cout << HEADER << item << endl;
    }

    void ingestAll(const BrowseItems &items) {
        for (BrowseItems::const_iterator itr = items.begin(); itr != items.end(); ++itr)
            ingest(*itr);
    }

    Playlist finalize() {
        return playlistBuilder;
    }

    bool empty() const {
        return clipIndex == 0;
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
    Transport transport;
};

static inline void parsePPL2(const CmdLinePlaylistBuilder::Pimpl &pimpl, const path &filename) {
    throw runtime_error(string("I can't parse PPL2 for the moment : ") + filename.string());
}

static inline void parsePPLSequenceLine(CmdLinePlaylistBuilder::Pimpl &pimpl, const string &line) {
    boost::escaped_list_separator<char> els("", "\t ", "\"");
    boost::tokenizer<boost::escaped_list_separator<char> > tok(line, els);
    vector<string> parts;
    copy(tok.begin(), tok.end(), back_inserter(parts));
    if (parts.size() != 4) {
        cout << HEADER + "discarding line " << line << endl;
        return;
    }
    const long record = atol(parts[0].c_str());
    const string &absoluteFilename = parts[1];
    const size_t separatorPosition = absoluteFilename.find_last_of("\\/");
    const string filename = separatorPosition == string::npos ? absoluteFilename : absoluteFilename.substr(separatorPosition + 1);
    const boost::filesystem::path path = separatorPosition == string::npos ? "" : absoluteFilename.substr(0, separatorPosition);
    const sequence::SequencePattern pattern = sequence::parsePattern(filename);
    const long first = atol(parts[2].c_str());
    const long last = atol(parts[3].c_str());
    if (record < 0 || first < 0 || last < 0)
        throw runtime_error("negative numbers are not allowed");
    pimpl.ingest(sequence::create_sequence(path, pattern, sequence::Range(first, last)), record);
}

static inline void parsePPL(CmdLinePlaylistBuilder::Pimpl &pimpl, const path &filename) {
    ifstream ppl(filename.string().c_str(), ios::in);
    string line;
    unsigned currentLine = 1;
    try {
        while (!ppl.eof()) {
            getline(ppl, line);
            boost::algorithm::trim(line);
            if (!line.empty() && line[0] != '#')
                parsePPLSequenceLine(pimpl, line);
            ++currentLine;
        }
    } catch (exception &e) {
        ostringstream oss;
        oss << filename << ':' << currentLine << ' ' << e.what();
        throw runtime_error(oss.str());
    }
}

static inline void parsePattern(const CmdLinePlaylistBuilder::Pimpl &pimpl, const path &filename) {
    throw runtime_error(string("I can't parse a filename pattern for the moment : ") + filename.string());
}

static inline void parseDirectory(CmdLinePlaylistBuilder::Pimpl &pimpl, const path &directory) {
    pimpl.ingestAll(sequence::parser::browse(directory.string().c_str(), false));
}

static inline bool contained(const string& filename, const BrowseItem & item) {
    return item.type == sequence::SEQUENCE && item.sequence.pattern.match(filename);
}

static inline bool notContained(const string& filename, const BrowseItem & item) {
    return !contained(filename, item);
}

static inline void parseFilename(CmdLinePlaylistBuilder::Pimpl &pimpl, const path &absoluteFilename) {
    BrowseItems items;
    items.push_back(sequence::create_file(absoluteFilename));
    if (pimpl.useContainingSequence) {
        items = sequence::parser::browse(absoluteFilename.parent_path().string().c_str(), false);
        const string filename = absoluteFilename.filename().string();
        sequence::filterOut(items, bind(&notContained, filename, _1));
        if (!items.empty()) {
            const BrowseItem &containingSequence = items[0];
            items.clear();
            items.push_back(containingSequence);
            assert(containingSequence.type==sequence::SEQUENCE);
            assert(containingSequence.sequence.step==1);
            const char * const pFrameString = filename.c_str() + containingSequence.sequence.pattern.prefix.size();
            const unsigned int filenameFrameNumber = atoi(pFrameString);
            const unsigned int sequenceOffset = filenameFrameNumber - containingSequence.sequence.range.first;
            const unsigned int gotoRec = pimpl.trackBuilder.currentRecord() + sequenceOffset;
            pimpl.transport.mutable_cue()->set_value(gotoRec);
            cout << HEADER + "cueing to record " << gotoRec << endl;
        }
    }
    pimpl.ingestAll(items);
}

static inline bool isPattern(const string &entry) {
    return entry.find_first_of("#@") != string::npos;
}

CmdLinePlaylistBuilder::CmdLinePlaylistBuilder(IOQueueInserter &inserter, bool useContainingSequence, const char **validExtensions) :
                m_Pimpl(new Pimpl(inserter, useContainingSequence, validExtensions)) {
}

Playlist CmdLinePlaylistBuilder::getPlaylist() {
    return m_Pimpl->finalize();
}

Transport CmdLinePlaylistBuilder::getCue() {
    return m_Pimpl->transport;
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

bool CmdLinePlaylistBuilder::empty() const {
    return m_Pimpl->empty();
}

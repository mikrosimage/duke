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
#include <boost/foreach.hpp>
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
static string HEADER("[Playlist] ");

enum EntryType {
    ET_PATTERN, ET_FILE, ET_PPL, ET_PPL2, ET_FOLDER
};

static inline bool isPattern(const string &entry) {
    return entry.find_first_of("#@") != string::npos;
}

EntryType getEntryType(const string &entry) {
    const path filename(entry);
    if (isPattern(entry))
        return ET_PATTERN;
    // it is either a Playlist, a filename or a directory, it therefore must exists
    if (!exists(filename))
        throw ios_base::failure(string("Unable to open ") + filename.string());
    if (iends_with(entry, g_ppl2))
        return ET_PPL2;
    else if (iends_with(entry, g_ppl))
        return ET_PPL;
    else if (is_directory(filename))
        return ET_FOLDER;
    else
        return ET_FILE;
}

struct CmdLinePlaylistBuilder::Pimpl : private boost::noncopyable {

    Pimpl(IOQueueInserter &inserter, bool browseMode, const char **validExtensions) :
                    inserter(inserter), browseMode(browseMode), trackBuilder(playlistBuilder.addTrack("default")), clipIndex(0) {
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

    void processEntry(const string& entry);

    Playlist finalize() {
        return playlistBuilder;
    }

    bool empty() const {
        return clipIndex == 0;
    }

    Transport getTransport() const {
        return transport;
    }

private:
    IOQueueInserter &inserter;
    const bool browseMode;
    PlaylistBuilder playlistBuilder;
    TrackBuilder trackBuilder;
    size_t clipIndex;
    Transport transport;
    set<string, details::ci_less> extensions;
    bool isInvalid(const BrowseItem &item) const {
        return extensions.find(item.extension()) == extensions.end();
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
        BOOST_FOREACH(const BrowseItem &item, items)
                    ingest(item);
    }

    void parsePPL2(const path &filename);
    void parsePPLSequenceLine(const string &line);
    void parsePPL(const path &filename);
    void parsePattern(const path &filename);
    void parseDirectory(const path &directory);
    void parseFilename(const path &absoluteFilename);
};

void CmdLinePlaylistBuilder::Pimpl::parsePPL2(const path &filename) {
    throw runtime_error(string("I can't parse PPL2 for the moment : ") + filename.string());
}

void CmdLinePlaylistBuilder::Pimpl::parsePPLSequenceLine(const string &line) {
    boost::escaped_list_separator<char> els("", "\t ", "\"");
    boost::tokenizer<boost::escaped_list_separator<char> > tok(line, els);
    vector<string> parts;
    copy(tok.begin(), tok.end(), back_inserter(parts));
    parts.erase(remove_if(parts.begin(), parts.end(), mem_fun_ref(&string::empty)), parts.end());
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
    ingest(sequence::create_sequence(path, pattern, sequence::Range(first, last)), record);
}

void CmdLinePlaylistBuilder::Pimpl::parsePPL(const path &filename) {
    ifstream ppl(filename.string().c_str(), ios::in);
    string line;
    unsigned currentLine = 1;
    try {
        while (!ppl.eof()) {
            getline(ppl, line);
            boost::algorithm::trim(line);
            if (!line.empty() && line[0] != '#')
                parsePPLSequenceLine(line);
            ++currentLine;
        }
    } catch (exception &e) {
        ostringstream oss;
        oss << filename << ':' << currentLine << ' ' << e.what();
        throw runtime_error(oss.str());
    }
}

void CmdLinePlaylistBuilder::Pimpl::parsePattern(const path &filename) {
    throw runtime_error(string("I can't parse a filename pattern for the moment : ") + filename.string());
}

void CmdLinePlaylistBuilder::Pimpl::parseDirectory(const path &directory) {
    ingestAll(sequence::parser::browse(directory.string().c_str(), false));
}

void CmdLinePlaylistBuilder::Pimpl::parseFilename(const path &absoluteFilename) {
    if (browseMode) {
        const string filename = absoluteFilename.filename().string();
        const boost::filesystem::path folder = absoluteFilename.parent_path();
        for (directory_iterator itr(folder), end; itr != end; ++itr) {
            if(!is_regular_file(*itr))
                continue;
            if(absoluteFilename==*itr)
                transport.mutable_cue()->set_value(trackBuilder.currentRecord());
            ingest(sequence::create_file(*itr));
        }
    } else {
        ingest(sequence::create_file(absoluteFilename));
    }
}

void CmdLinePlaylistBuilder::Pimpl::processEntry(const string& entry) {
    const EntryType type = getEntryType(entry);
    const path filename(entry);
    if (browseMode && type != ET_FILE)
        throw std::logic_error("Browse mode require a file as argument");

    switch (type) {
        case ET_PATTERN:
            parsePattern(filename);
            break;
        case ET_PPL2:
            parsePPL2(filename);
            break;
        case ET_PPL:
            parsePPL(filename);
            break;
        case ET_FOLDER:
            parseDirectory(filename);
            break;
        case ET_FILE:
            parseFilename(filename);
            break;
    }
}

CmdLinePlaylistBuilder::CmdLinePlaylistBuilder(IOQueueInserter &inserter, bool browseMode, const char **validExtensions) :
                m_Pimpl(new Pimpl(inserter, browseMode, validExtensions)) {
}

Playlist CmdLinePlaylistBuilder::getPlaylist() {
    return m_Pimpl->finalize();
}

Transport CmdLinePlaylistBuilder::getCue() {
    return m_Pimpl->getTransport();
}

void CmdLinePlaylistBuilder::process(const string& entry) {
    m_Pimpl->processEntry(entry);
}

bool CmdLinePlaylistBuilder::empty() const {
    return m_Pimpl->empty();
}

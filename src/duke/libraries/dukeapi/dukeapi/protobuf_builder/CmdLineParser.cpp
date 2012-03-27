/*
 * CmdLineParser.cpp
 *
 *  Created on: 27 mars 2012
 *      Author: Guillaume Chatelet
 */

#include "CmdLineParser.h"

#include <sequence/parser/Browser.h>
#include <sequence/BrowseItem.h>
#include <sequence/Sequence.h>

#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <boost/tokenizer.hpp>

#include <sstream>

using namespace std;
using namespace duke::playlist;

static string g_ppl2(".ppl2");
static string g_ppl(".ppl");
static string HEADER("[Playlist] ");

static size_t currentRecord(const Playlist &playlist) {
    if (playlist.shot_size() == 0)
        return 0;
    const Shot &lastShot = playlist.shot().Get(playlist.shot_size() - 1);
    if (lastShot.has_trackend())
        return lastShot.trackend() + 1;
    if (lastShot.has_mediastart())
        return lastShot.trackend() + lastShot.mediaend() - lastShot.mediastart() + 1;
    return 0;
}
static void addMedia(Playlist &playlist, const sequence::BrowseItem &item, size_t record = UINT_MAX) {
    if (item.type == sequence::SEQUENCE && item.sequence.step > 1) {
        const sequence::Sequence &sequence = item.sequence;
        for (size_t i = sequence.range.first; i <= sequence.range.last; i += sequence.step)
            addMedia(playlist, sequence::create_file(item.path / sequence::instanciatePattern(sequence.pattern, i)));
        return;
    }
    if (record == UINT_MAX)
        record = currentRecord(playlist);
    Shot &shot = *playlist.add_shot();
    size_t duration = 0;
    switch (item.type) {
        case sequence::SEQUENCE: {
            boost::filesystem::path preferred(item.path / item.sequence.pattern.string());
            preferred.make_preferred();
            shot.set_media(preferred.string());
            const sequence::Range &range(item.sequence.range);
            shot.set_mediastart(range.first);
            shot.set_mediaend(range.last);
            duration = range.last-range.first;
            break;
        }
        case sequence::UNITFILE: {
            boost::filesystem::path preferred(item.path);
            preferred.make_preferred();
            shot.set_media(preferred.string());
            break;
        }
        default:
            throw runtime_error("can't convert BrowseItem to media");
    }
    shot.set_trackstart(record);
    shot.set_trackend(record + duration);
}

static bool contained(const sequence::BrowseItem &item, const string &filename) {
    if (item.type != sequence::SEQUENCE)
        return false;
    const sequence::SequencePattern &pattern = item.sequence.pattern;
    return boost::starts_with(filename, pattern.prefix) && boost::ends_with(filename, pattern.suffix);
}

static bool notContained(const sequence::BrowseItem &item, const string &filename) {
    return !contained(item, filename);
}

static const string selectFilename(const sequence::BrowseItem &item) {
    if (item.type == sequence::SEQUENCE)
        return item.sequence.pattern.string();
    if (item.type == sequence::UNITFILE)
        return item.path.filename().string();
    return "";
}

static bool browseItemLess(const sequence::BrowseItem &a, const sequence::BrowseItem &b) {
    return selectFilename(a) < selectFilename(b);
}

static string makeAbsolute(boost::filesystem::path path) {
    if (path.is_relative())
        path = boost::filesystem::current_path() / path;
    path.normalize().make_preferred();
    return path.string();
}

static string getParent(boost::filesystem::path path) {
    return path.parent_path().string();
}

struct CmdLineParser {
    CmdLineParser(const extension_set &supportedFormat) :
                    supportedFormat(supportedFormat) {
        playlistFormat.insert(g_ppl);
        playlistFormat.insert(g_ppl2);
    }

    Playlist browseViewer(const string& filename) {
        checkValidImage(filename);
        Playlist playlist;
        addMedia(playlist, sequence::create_file(filename));
        return playlist;
    }

    Playlist browseViewerComplete(const string& filename) {
        checkValidImage(filename);
        Playlist playlist;
        handleDirectory(playlist, getParent(filename));
        const string absoluteFilename = makeAbsolute(filename);
        typedef google::protobuf::RepeatedPtrField<duke::playlist::Shot> Shots;
        for (Shots::const_iterator itr = playlist.shot().begin(), end = playlist.shot().end(); itr != end; ++itr) {
            if (itr->media() == absoluteFilename) {
                playlist.set_startframe(itr->trackstart());
                break;
            }
        }
        return playlist;
    }

    Playlist browsePlayer(const vector<string>& args) {
        Playlist playlist;
        for_each(args.begin(), args.end(), boost::bind(&CmdLineParser::browsePlayerArgument, this, boost::ref(playlist), _1));
        return playlist;
    }
private:
    void checkValidImage(const string& filename) const {
        if (!boost::filesystem::is_regular_file(filename))
            throw cmdline_exception(HEADER + "The provided argument is not a file : " + filename);
        if (!supportedFormat.match(filename))
            throw cmdline_exception(HEADER + "The provided argument is not supported : " + filename);
    }

    void browsePlayerArgument(Playlist &playlist, const string &arg) {
        boost::filesystem::path path(arg);
        if (boost::filesystem::is_directory(arg))
            handleDirectory(playlist, arg);
        else if (boost::filesystem::is_regular(arg)) {
            if (boost::iends_with(arg, g_ppl))
                handlePPL(playlist, arg);
            else if (boost::iends_with(arg, g_ppl2))
                handlePPL2(playlist, arg);
            else
                handleFile(playlist, path);
        } else
            throw cmdline_exception(HEADER + "The provided argument is not a file nor a directory : " + arg);
        // handle pattern too
    }

    void handlePPL(Playlist &playlist, const string &filename) {
        ifstream ppl(filename.c_str(), ios::in);
        string line;
        unsigned currentLine = 1;
        try {
            while (!ppl.eof()) {
                getline(ppl, line);
                boost::algorithm::trim(line);
                if (!line.empty() && line[0] != '#')
                    parsePPLSequenceLine(playlist, line);
                ++currentLine;
            }
        } catch (exception &e) {
            ostringstream oss;
            oss << filename << ':' << currentLine << ' ' << e.what();
            throw runtime_error(oss.str());
        }
    }

    void parsePPLSequenceLine(Playlist &playlist, const string &line) {
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
            throw cmdline_exception(HEADER + "Negative numbers are not allowed at line '" + line + '\'');
        addMedia(playlist, sequence::create_sequence(path, pattern, sequence::Range(first, last)), record);
    }

    void handlePPL2(Playlist &playlist, const string &path) {
        throw cmdline_exception(HEADER + "PPL2 are not supported for the moment");
    }

    void handleFile(Playlist &playlist, boost::filesystem::path path) {
        path = makeAbsolute(path);
        checkValidImage(path.string());
        sequence::BrowseItems items = sequence::parser::browse(getParent(path).c_str());
        sequence::filterOut(items, boost::bind(&notContained, _1, path.filename().string()));
        addMedia(playlist, items.empty() ? sequence::create_file(path.string()) : items[0]);
    }

    void handleDirectory(Playlist &playlist, const string &path) {
        sequence::BrowseItems items = sequence::parser::browse(path.c_str());
        sequence::filterOut(items, boost::bind(&CmdLineParser::isNotSupported, this, _1));
        sort(items.begin(), items.end(), &::browseItemLess);
        for_each(items.begin(), items.end(), boost::bind(&::addMedia, boost::ref(playlist), _1, UINT_MAX));
    }

    bool isNotSupported(const sequence::BrowseItem &a) {
        return a.type == sequence::UNDEFINED || a.type == sequence::FOLDER || supportedFormat.match(a.extension()) == false;
    }

    extension_set playlistFormat;
    const extension_set supportedFormat;
};

Playlist browsePlayer(const extension_set &valid_extensions, const std::vector<std::string> &inputs) {
    return CmdLineParser(valid_extensions).browsePlayer(inputs);
}
Playlist browseViewer(const extension_set &valid_extensions, const std::string &input) {
    return CmdLineParser(valid_extensions).browseViewer(input);
}
Playlist browseViewerComplete(const extension_set &valid_extensions, const std::string &input) {
    return CmdLineParser(valid_extensions).browseViewerComplete(input);
}

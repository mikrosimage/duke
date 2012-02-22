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

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>
#include <boost/bind.hpp>

#include <stdexcept>
#include <string>
#include <vector>
#include <set>

#include <algorithm>

using namespace std;
using namespace boost::algorithm;
using namespace boost::filesystem;
using namespace duke::protocol;

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
                    useContainingSequence(useContainingSequence) {
        for (; validExtensions != NULL && *validExtensions != NULL; ++validExtensions) {
            string extension = *validExtensions;
            if (!extension.empty() && extension[0] != '.')
                extension.insert(extension.begin(), '.');
            extensions.insert(extension);
        }
    }
    struct IsInvalidExtension {
        const Pimpl * const ptr;
        IsInvalidExtension(const Pimpl * ptr) :
                        ptr(ptr) {
        }
        inline bool operator()(const sequence::BrowseItem &item) const {
            return ptr->extensions.find(item.extension()) == ptr->extensions.end();
        }
    };
    IsInvalidExtension isInvalid() const {
        return IsInvalidExtension(this);
    }

    const bool useContainingSequence;
    PlaylistBuilder playlistBuilder;
private:
    set<string, details::ci_less> extensions;
};

static inline void parsePPL2(const CmdLinePlaylistBuilder::Pimpl &pimpl, const path &filename) {
}

static inline void parsePPL(const CmdLinePlaylistBuilder::Pimpl &pimpl, const path &filename) {
}

static inline void parsePattern(const CmdLinePlaylistBuilder::Pimpl &pimpl, const path &filename) {
}

static inline void parseFilename(const CmdLinePlaylistBuilder::Pimpl &pimpl, const path &filename) {
    const bool isFile = is_regular_file(filename);
    if (isFile && !pimpl.useContainingSequence) {
        return;
    }
    path toBrowse = isFile ? filename.parent_path() : filename;
    vector<sequence::BrowseItem> items = sequence::parser::browse(toBrowse.string().c_str(), false);
    items.erase(remove_if(items.begin(), items.end(), pimpl.isInvalid()), items.end());
}

static inline bool isPattern(const string &entry) {
    return entry.find_first_of("#@") != string::npos;
}

CmdLinePlaylistBuilder::CmdLinePlaylistBuilder(bool useContainingSequence, const char **validExtensions) :
                m_Pimpl(new Pimpl(useContainingSequence, validExtensions)) {
}

void CmdLinePlaylistBuilder::operator ()(const string& entry) {
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
    else
        parseFilename(pimpl, filename);
}

#include "File.hpp"
#include <iomanip>
#include <boost/regex.hpp>

namespace bfs = boost::filesystem;

std::ostream& File::getCout(std::ostream& os) const {
    bfs::path dir;
    if (showAbsolutePath()) {
        dir = bfs::absolute(_directory);
        dir = boost::regex_replace(dir.string(), boost::regex("/\\./"), "/");
    }
    os << std::left;
    if (showProperties()) {
        os << std::setw(PROPERTIES_WIDTH) << "f ";
    }
    if (showRelativePath()) {
        dir = _directory;
        dir = boost::regex_replace(dir.string(), boost::regex("/\\./$"), "/");
        std::string path = (dir / _filename).string();

        os << std::setw(NAME_WIDTH_WITH_DIR) << _kColorFile + path + _kColorStd;
    } else {
        os << std::setw(NAME_WIDTH) << _kColorFile + (dir / _filename).string() + _kColorStd;
    }
    return os;
}

std::vector<boost::filesystem::path> File::getFiles() const {
    std::vector<boost::filesystem::path> allPaths;
    allPaths.push_back(_directory / _filename);
    return allPaths;
}

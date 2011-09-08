#include "Folder.hpp"
#include <iomanip>
#include <boost/regex.hpp>

namespace bfs = boost::filesystem;

// do not work actually ....
std::ostream& Folder::getProperties(std::ostream& os, const boost::filesystem::path& directory) {
    os << "d " << boost::filesystem::file_size(directory);
    return os;
}

std::ostream& Folder::getCout(std::ostream& os) const {
    bfs::path dir;
    if (showAbsolutePath()) {
        dir = bfs::absolute(_directory);
        dir = boost::regex_replace(dir.string(), boost::regex("/\\./"), "/");
    }
    os << std::left;
    if (showProperties()) {
        os << std::setw(PROPERTIES_WIDTH) << "d ";
    }
    if (showRelativePath()) {
        dir = _directory;
        dir = boost::regex_replace(dir.string(), boost::regex("/\\./"), "/");
        std::string path = (dir / _folderName).string();
        os << std::setw(NAME_WIDTH_WITH_DIR) << _kColorFolder + path + _kColorStd;
    } else {
        os << std::setw(NAME_WIDTH) << _kColorFolder + (dir / _folderName).string() + _kColorStd;
    }
    return os;
}

std::vector<boost::filesystem::path> Folder::getFiles() const {
    std::vector<boost::filesystem::path> allPaths;
    allPaths.push_back(_directory / _folderName);
    return allPaths;
}

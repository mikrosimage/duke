#include "FileObject.hpp"

FileObject::~FileObject() {
}

std::ostream& operator<<(std::ostream& os, const FileObject& fo) {
    fo.getCout(os);
    return os;
}

void FileObject::setDirectoryFromPath(const boost::filesystem::path& p) {
    // if it's not a directory, use the parent directory of the file
    _directory = p.parent_path();
    if (_directory.empty()) { // relative path
        _directory = boost::filesystem::current_path();
    }
}

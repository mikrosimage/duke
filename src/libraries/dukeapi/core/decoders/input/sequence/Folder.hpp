#ifndef _FOLDER_HPP_
#define _FOLDER_HPP_

#include "FileObject.hpp"

class Folder : public FileObject {

public:
    Folder(const boost::filesystem::path& directory, const std::string folderName, const EMaskOptions& options) :
        FileObject(directory, eMaskTypeDirectory, options) {
        _folderName = folderName;
    }

    ~Folder() {
    }

    std::ostream& getCout(std::ostream& os) const;

    std::vector<boost::filesystem::path> getFiles() const;

    inline void clear() {
        FileObject::clear();
        _folderName.clear();
    }

private:
    std::ostream& getProperties(std::ostream& os, const boost::filesystem::path& directory);

private:
    std::string _folderName;
};

#endif // _FOLDER_HPP_

#ifndef _FILEOBJECT_HPP_
#define _FILEOBJECT_HPP_

#include <boost/filesystem.hpp>

#define PROPERTIES_WIDTH 3
#define NAME_WIDTH 50
#define NAME_WIDTH_WITH_DIR 80

#ifdef __LINUX__
static const std::string kColorStd ( "\E[0;0m" );
static const std::string kColorFolder ( "\E[1;34m" );
static const std::string kColorFile ( "\E[0;32m" );
static const std::string kColorSequence ( "\E[0;32m" );
static const std::string kColorError ( "\E[1;31m" );
#else
static const std::string kColorStd("");
static const std::string kColorFolder("");
static const std::string kColorFile("");
static const std::string kColorSequence("");
static const std::string kColorError("");
#endif

namespace { // empty namespace

/**
 * List all recognized pattern types.
 */
enum EMaskType {
    eMaskTypeUndefined = 0,
    eMaskTypeDirectory = 1,
    eMaskTypeFile = eMaskTypeDirectory * 2,
    eMaskTypeSequence = eMaskTypeFile * 2,
    eMaskTypeDefault = eMaskTypeSequence
};

enum EMaskOptions {
    eMaskOptionsNone = 0, // 0
    eMaskOptionsProperties = 1, // show type of FileObject
    eMaskOptionsPath = eMaskOptionsProperties * 2, // show path of FileObject
    eMaskOptionsAbsolutePath = eMaskOptionsPath * 2, // show absolute path of FileObject
    eMaskOptionsDotFile = eMaskOptionsAbsolutePath * 2, // show files which start with a dot (hidden files)
    eMaskOptionsColor = eMaskOptionsDotFile * 2, // output with color
    eMaskOptionsDefault = (eMaskOptionsPath | eMaskOptionsColor)
};

inline EMaskType operator~(const EMaskType& a) {
    EMaskType b = (EMaskType) (~int(a));
    return b;
}

inline EMaskType operator&=(EMaskType& a, const EMaskType& b) {
    a = (EMaskType) (int(b) & int(a));
    return a;
}

inline EMaskType operator|=(EMaskType& a, const EMaskType& b) {
    a = (EMaskType) (int(b) | int(a));
    return a;
}

inline EMaskOptions operator|=(EMaskOptions& a, const EMaskOptions& b) {
    a = (EMaskOptions) (int(b) | int(a));
    return a;
}

inline EMaskOptions remove(EMaskOptions& a, const EMaskOptions& b) {
    a = (EMaskOptions) (int(~b) & int(a));
    return a;
}

} // empty namespace

/**
 * @brief A container for files, directories and sequences.
 */
class FileObject {

public:
    FileObject() {
        _directory.clear();
        _type = eMaskTypeUndefined;
        _options = eMaskOptionsNone;
        setColorActive(_options & eMaskOptionsColor);
    }

    FileObject(const EMaskOptions options) {
        _directory.clear();
        _type = eMaskTypeUndefined;
        _options = options;
        setColorActive(_options & eMaskOptionsColor);
    }

    /**
     * @brief Construct a FileObject with given informations.
     */
    FileObject(const boost::filesystem::path& directory, const EMaskType& type, const EMaskOptions& options) {
        init(directory, type, options);
    }

    virtual ~FileObject();

    friend std::ostream& operator<<(std::ostream& os, const FileObject& fo);

    virtual std::ostream& getCout(std::ostream& os) const = 0;

    /**
     * @todo: can we remove this?
     */
    virtual std::vector<boost::filesystem::path> getFiles() const =0;

    /// @todo
    // virtual std::string getName() const = 0;
    //  boost::filesystem::path getAbsoluteName() const
    //  {
    //      return getDirectory() / getName();
    //  }

    inline boost::filesystem::path getDirectory() const {
        return _directory;
    }

    inline boost::filesystem::path getAbsoluteDirectory() const {
        return boost::filesystem::absolute(_directory);
    }

    inline void setDirectory(const boost::filesystem::path& p) {
        _directory = p;
    }

    void setDirectoryFromPath(const boost::filesystem::path& p);

    EMaskOptions getMaskOptions() const {
        return _options;
    }

    EMaskType getMaskType() const {
        return _type;
    }

    virtual inline void clear() {
        _directory.clear();
        _type = eMaskTypeDefault;
        _options = eMaskOptionsDefault;
    }

private:
    void init(const boost::filesystem::path& directory, const EMaskType& type, const EMaskOptions& options) {
        _directory = directory;
        _type = type;
        _options = options;
        setColorActive(_options & eMaskOptionsColor);
    }

    void setColorActive(bool activate = false) {
        if (activate) {
            _kColorStd = kColorStd;
            _kColorFolder = kColorFolder;
            _kColorFile = kColorFile;
            _kColorSequence = kColorSequence;
            _kColorError = kColorError;
        } else {
            _kColorStd = "";
            _kColorFolder = "";
            _kColorFile = "";
            _kColorSequence = "";
            _kColorError = "";
        }
    }

protected:
    inline bool showProperties() const {
        return _options & eMaskOptionsProperties;
    }

    inline bool showRelativePath() const {
        return _options & eMaskOptionsPath;
    }

    inline bool showAbsolutePath() const {
        return _options & eMaskOptionsAbsolutePath;
    }

protected:
    boost::filesystem::path _directory; ///< directory
    EMaskType _type; ///< specify type of object
    EMaskOptions _options; ///< specify output options of object, common for each objects
    std::string _kColorStd;
    std::string _kColorFolder;
    std::string _kColorFile;
    std::string _kColorSequence;
    std::string _kColorError;
};

#endif // _FILEOBJECT_HPP_

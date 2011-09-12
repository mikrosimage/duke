#ifndef FILEGRABBER_H_
#define FILEGRABBER_H_

#include <boost/filesystem.hpp>
#include <vector>
#include <utility>

typedef std::vector< std::pair< ::boost::filesystem::path, size_t > > PATH_VECTOR;

class FileGrabber
{
private:
	PATH_VECTOR m_vPaths;

public: FileGrabber( const char* directory );
	~FileGrabber();

	const PATH_VECTOR& getPaths() const;
};

#endif /* FILEGRABBER_H_ */

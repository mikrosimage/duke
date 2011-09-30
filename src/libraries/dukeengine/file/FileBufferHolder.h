#ifndef FILEBUFFERHOLDER_H_
#define FILEBUFFERHOLDER_H_

#include <boost/noncopyable.hpp>
#include <boost/filesystem/path.hpp>
#include <vector>

class PlaylistHelper;
class ImageHolder;
class ImageReader;
class SmartCache;

class FileBufferHolder : public boost::noncopyable
{
private:
	typedef std::vector< ImageHolder > Images;
	const ImageReader &m_ImageReader;
	Images m_Images;
    std::vector<size_t> m_Iterators;
    size_t m_LastFrame;

public:
	FileBufferHolder(const ImageReader&);

	void update(const uint32_t frame, const PlaylistHelper &);
	void update(const uint32_t frame, const PlaylistHelper &, const SmartCache&);

	const Images& getImages() const;
};

#endif /* FILEBUFFERHOLDER_H_ */

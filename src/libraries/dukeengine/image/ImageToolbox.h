#ifndef IMAGETOOLBOX_H_
#define IMAGETOOLBOX_H_

#include <dukeengine/chain/Chain.h>
#include <dukeengine/image/ImageHolder.h>
#include <dukeengine/sequence/PlaylistHelper.h>
#include <dukeengine/host/io/ImageDecoderFactory.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/shared_ptr.hpp>

/**
 * This struct converts a hash back to a filename
 */
struct PlaylistHashToName {
    const SharedPlaylistHelperPtr playlistHelper;
    PlaylistHashToName(const SharedPlaylistHelperPtr &playlistHelper);
    std::string getFilename(uint64_t hash) const;
};

/**
 * Our implementation of slot data, it simply holds an ImageHolder
 * but we can put more data in it if needed.
 */
struct ASlotData : public SlotData {
    MemoryBlockPtr m_pFileMemoryBlock;
    ImageDescription m_TempImageDescription;
    ImageHolder m_Holder;
    std::string m_Filename;
    std::string m_FilenameExtension;
    bool m_bDelegateReadToHost;
    bool m_bFormatUncompressed;
    FormatHandle m_FormatHandler;
    ::boost::posix_time::time_duration loadTime;
    ::boost::posix_time::time_duration decodeTime;
};
typedef boost::shared_ptr<ASlotData> TSlotDataPtr;

/**
 * Definition of the workers
 */
//void loadWorker(Chain &chain, const ImageDecoderFactory& factory, const unsigned cpu);
//void decodeWorker(Chain &chain, const ImageDecoderFactory& factory, const unsigned cpu);


struct load_error : public std::runtime_error {
    load_error(std::string what) :
        std::runtime_error(what) {
    }
};

uint64_t getNextFilename(Chain &chain, TSlotDataPtr& pData);
void getImageHandler(const ImageDecoderFactory& factory, TSlotDataPtr& pData);
void loadFileFromDisk(TSlotDataPtr& pData);
void readHeader(const ImageDecoderFactory& factory, TSlotDataPtr& pData);
void readImage(const ImageDecoderFactory& imageFactory, TSlotDataPtr& pData);
bool isAlreadyUncompressed(TSlotDataPtr &pData);


#endif /* IMAGETOOLBOX_H_ */

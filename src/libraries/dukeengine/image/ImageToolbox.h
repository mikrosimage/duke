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
struct DukeSlot : public SlotData {
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
typedef boost::shared_ptr<DukeSlot> TSlotDataPtr;

/**
 * Definition of the workers
 */
//void loadWorker(Chain &chain, const ImageDecoderFactory& factory, const unsigned cpu);
//void decodeWorker(Chain &chain, const ImageDecoderFactory& factory, const unsigned cpu);

/**
 * A load_error exception will be thrown if something happens
 * during image loading
 */
struct load_error : public std::runtime_error {
    load_error(std::string what) :
        std::runtime_error(what) {
    }
};

/**
 * This function will pick a slot to load from the chain
 * and fill the filename, filename extension and hash
 * of the provided slot.
 */
uint64_t setupFilename(TSlotDataPtr& forSlot, Chain &withChain);

/**
 * This function will try to find a loader for the format.
 * It will then setup the Slot accordingly, setting the fields
 * m_bDelegateReadToHost and m_bFormatUncompressed
 */
void setupLoaderInfo(TSlotDataPtr& forSlot, const ImageDecoderFactory& withFactory);

/**
 * This function will use the set ImageLoader to load the file
 * and set the pFileData and fileDataSize fields of m_TempImageDescription
 */
void loadFileAndDecodeHeader(TSlotDataPtr& forSlot, const ImageDecoderFactory& withFactory);

/**
 * Combines setupFilename and setupLoaderInfo with some
 */
void loadFileFromDisk(TSlotDataPtr& pData);
void readImage(const ImageDecoderFactory& imageFactory, TSlotDataPtr& pData);
bool isAlreadyUncompressed(TSlotDataPtr &pData);

#endif /* IMAGETOOLBOX_H_ */

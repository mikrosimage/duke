#define DUKE_LIBAV
#ifdef DUKE_LIBAV

#include <duke/base/NonCopyable.hpp>
#include <duke/imageio/DukeIO.hpp>
#include <duke/attributes/AttributeKeys.hpp>
#include <duke/gl/GL.hpp>

#include <mutex>
#include <memory>
#include <vector>
#include <iostream>
#include <fstream>

#ifdef __cplusplus
extern "C" {
#endif
#include <libavutil/avutil.h>
#include <libavutil/frame.h>
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>

#ifdef __cplusplus
}
#endif

using namespace std;

namespace std {

template<> struct default_delete<AVFormatContext> {
    void operator()(AVFormatContext *ptr) {
        if (ptr) avformat_free_context(ptr);
    }
};
template<> struct default_delete<AVCodecContext> {
    void operator()(AVCodecContext *ptr) {
        if (ptr) avcodec_close(ptr);
    }
};
template<> struct default_delete<AVFrame> {
    void operator()(AVFrame *ptr) {
        if (ptr) av_frame_free(&ptr);
    }
};

}

namespace {

bool success(int result) {
    return result >= 0;
}

bool fail(int result) {
    return result < 0;
}

void check(bool success, const char * message) {
    if (!success) {
        throw runtime_error(message);
    }
}

void check(int result, const char * message) {
    check(success(result), message);
}

struct IndexEntry {
    int64_t pos;
    int64_t timestamp;
    size_t keyframeIndex;
};

class Index {
public:
    Index(const AVIndexEntry * begin, const size_t count) {
        m_Index.reserve(count);
        size_t lastKeyFrame = 0;
        for (const AVIndexEntry * end = begin + count; begin != end; ++begin) {
            const bool keyframe = begin->flags > 0;
            if (keyframe) lastKeyFrame = m_Index.size();
            m_Index.push_back( { begin->pos, begin->timestamp, lastKeyFrame });
        }
    }
    size_t getFrameCount() const {
        return m_Index.size();
    }
    const IndexEntry& getEntryAt(size_t frame) const {
        return m_Index.at(frame);
    }
    const std::vector<IndexEntry>& getEntries() const {
        return m_Index;
    }
    size_t getFrameFromTimestamp(int64_t ts) const {
        auto itr = lower_bound(m_Index.begin(), m_Index.end(), IndexEntry(), [=](const IndexEntry& a,const IndexEntry&) {
            return a.timestamp<ts;});
        if (itr == m_Index.end()) return getFrameCount() - 1;
        const size_t index = distance(m_Index.begin(), itr);
        return getEntryAt(index).timestamp == ts ? index : index - 1;
    }
private:
    std::vector<IndexEntry> m_Index;
};

struct PacketHolder : public noncopyable {
    PacketHolder() {
        init();
    }

    bool isValid() const {
        return pkt.size > 0 && pkt.data != nullptr;
    }

    ~PacketHolder() {
        destroy();
    }

    AVPacket* getPacketPtr() {
        return &pkt;
    }

    void reset() {
        destroy();
        init();
    }

    void checkValid() const {
        check(pkt.size > 0 && pkt.data != nullptr, "Invalid packet");
    }

    void print() const {
        printf("packet\tpos:%ld\tpts:%ld\tdts:%ld\n", pkt.pos, pkt.pts, pkt.dts);
    }

private:
    void destroy() {
        if (pkt.buf) av_free_packet(&pkt);
    }

    void init() {
        av_init_packet(&pkt);
        pkt.data = nullptr;
        pkt.size = 0;

    }

    AVPacket pkt;
};

struct StreamPacketReader {
    StreamPacketReader(const int streamIndex, AVFormatContext *pFormatContext) :
                    m_StreamIndex(streamIndex), m_pFormatContext(pFormatContext), m_EndOfStream(false) {
        loadNextPacket();
    }

    AVPacket* getCurrentPacket() {
        m_Holder.checkValid();
        return m_Holder.getPacketPtr();
    }

    void loadNextPacket() {
        AVPacket* pPacket = m_Holder.getPacketPtr();
        do {
            m_EndOfStream = fail(av_read_frame(m_pFormatContext, pPacket));
        } while (!m_EndOfStream && pPacket->stream_index != m_StreamIndex);
    }

    bool endOfStream() const {
        return m_EndOfStream;
    }

    // Don't forget to avcodec_flush_buffers after seeking
    bool seekToStreamTimestamp(int64_t ts) {
        check(avformat_seek_file(m_pFormatContext, m_StreamIndex, ts, ts, ts, AVSEEK_FLAG_ANY), "can't seek to the specified timestamp");
        // Clearing current packet after a seek
        m_Holder.reset();
        loadNextPacket();
        return !endOfStream();
    }

    void printPacket() const {
        m_Holder.print();
    }
private:
    PacketHolder m_Holder;
    int m_StreamIndex;
    AVFormatContext *m_pFormatContext;
    bool m_EndOfStream;
};

struct MovieContainer {
    MovieContainer(const char* filename) :
                    m_pFormatCtx(avformat_alloc_context()) {
        AVFormatContext *pFormatContext = getFormatPtr();
        check(avformat_open_input(&pFormatContext, filename, nullptr, nullptr), "cannot open file");
        check(avformat_find_stream_info(pFormatContext, 0), "cannot enumerate streams");
        av_dump_format(pFormatContext, 0, filename, 0);
    }
    AVFormatContext *getFormatPtr() const {
        return m_pFormatCtx.get();
    }

private:
    std::unique_ptr<AVFormatContext> m_pFormatCtx;
};

struct Stream {
    Stream(const MovieContainer& container) :
                    m_Container(container), //
                    m_StreamIndex(av_find_default_stream_index(m_Container.getFormatPtr())), //
                    m_pStream(m_Container.getFormatPtr()->streams[m_StreamIndex]), //
                    m_Index(m_pStream->index_entries, m_pStream->nb_index_entries), //
                    m_FirstFrame(m_Index.getFrameFromTimestamp(m_pStream->start_time)), //
                    m_LastFrame(m_Index.getFrameFromTimestamp(std::numeric_limits<int64_t>::max())) {
//        printf("\nframe\tpos\tpts\tkeyframe\n");
//        size_t i = 0;
//        for (const auto entry : m_Index.getEntries()) {
//            printf("%lu\t%ld\t%ld\t%d\n", i, entry.pos, entry.timestamp, entry.keyframeIndex == i);
//            ++i;
//        }
        if (m_Index.getEntries().size() <= 1) {
            throw std::runtime_error("no exploitable index in file");
        }
        printf("Stream starts at ts %ld\n", m_pStream->start_time);
    }
    AVFormatContext *getFormatPtr() const {
        return m_Container.getFormatPtr();
    }
    const AVStream* getStreamPtr() const {
        return m_pStream;
    }
    size_t getIndex() const {
        return m_StreamIndex;
    }
    const Index& getContainerIndex() const {
        return m_Index;
    }
    size_t getFirstFrame() const {
        return m_FirstFrame;
    }
    size_t getLastFrame() const {
        return m_LastFrame;
    }
    size_t getFrameFromTimestamp(int64_t ts) const {
        return m_Index.getFrameFromTimestamp(ts) - m_FirstFrame;
    }
private:
    const MovieContainer& m_Container;
    const size_t m_StreamIndex;
    const AVStream* m_pStream;
    const Index m_Index;
    const size_t m_FirstFrame;
    const size_t m_LastFrame;
};

struct StreamFrameDecoder {
    StreamFrameDecoder(const Stream& stream) :
                    m_Stream(stream), //
                    m_pCodecCtx(stream.getStreamPtr()->codec), //
                    m_PacketReader(stream.getIndex(), stream.getFormatPtr()), //
                    m_pFrameHolder(av_frame_alloc()), m_CurrentFrame(-1) {
        // finding codec
        const AVCodec * const pCodec = avcodec_find_decoder(m_pCodecCtx->codec_id);
        check(pCodec, "codec not found");
        // opening codec
        check(avcodec_open2(m_pCodecCtx, pCodec, 0), "cannot open decoder");
        decodeNextFrame();
    }

    const AVFrame* getCurrentFramePtr() const {
        return m_pFrameHolder.get();
    }

    size_t getCurrentFrame() const {
        return m_CurrentFrame;
    }

    void decodeNextFrame() {
//        printf("decoding frame : start\n");
        AVFrame* pFrame = m_pFrameHolder.get();
        while (true) {
            AVPacket* pPacket = m_PacketReader.getCurrentPacket();
//            m_PacketReader.printPacket();
            int gotFrame = 0;
            const int decodedBytes = avcodec_decode_video2(m_pCodecCtx, pFrame, &gotFrame, pPacket);
            if (fail(decodedBytes)) {
                const bool noMorePackets = m_PacketReader.endOfStream();
                if (noMorePackets)
                    throw runtime_error("end of stream while decoding image");
                else
                    throw runtime_error("unable to decode image");
            }
            // the whole packet should be decoded at once
            check(decodedBytes >= 0 && decodedBytes == pPacket->size, "invalid decoded byte count");
            // decoding next packet in any case
            // - image not yet decoded, we must use next packet
            // - image decoded, we prepare for next decode cycle
            m_PacketReader.loadNextPacket();
            if (gotFrame) {
                const auto ts = m_pFrameHolder->best_effort_timestamp;
                if (ts == AV_NOPTS_VALUE) throw runtime_error("corrupted frame");
                m_CurrentFrame = m_Stream.getFrameFromTimestamp(m_pFrameHolder->best_effort_timestamp);
//                printf("decoding frame : end : frame:%lu\tpts:%ld\tbets:%ld\n", m_CurrentFrame, m_pFrameHolder->pts, m_pFrameHolder->best_effort_timestamp);
                return;
            }
        }
    }

    bool endOfStream() const {
        return m_PacketReader.endOfStream();
    }

    // frame here should take into account stream startFrame
    // ie. if stream start frame is 2 you must not ask for frame 0 or 1
    void decodeFrame(size_t frame) {
        check(frame >= m_Stream.getFirstFrame(), "frame must be greater or equals to stream first frame");
        check(frame <= m_Stream.getLastFrame(), "frame must be less or equals to stream last frame");
        if (frame == m_CurrentFrame) return;
        const auto getEntry = [&](size_t frame) {
            return m_Stream.getContainerIndex().getEntryAt(frame);
        };
        const auto getFrameTimestamp = [&](size_t frame) {
            return getEntry(frame).timestamp;
        };
        const auto getKeyframeTimestamp = [&](size_t frame) {
            return getEntry(getEntry(frame).keyframeIndex).timestamp;
        };
        const auto frameTs = getFrameTimestamp(frame);
        const auto keyframeTs = getKeyframeTimestamp(frame);

        const auto lastFrameTs = getFrameTimestamp(m_CurrentFrame);
        const auto lastKeyframeTs = getKeyframeTimestamp(m_CurrentFrame);

        const bool fastForward = frameTs > lastFrameTs && keyframeTs == lastKeyframeTs;
        const bool mustSeek = !fastForward;

        if (mustSeek) { // no choice but seeking
            if (!m_PacketReader.seekToStreamTimestamp(keyframeTs)) throw runtime_error("can't seek to requested frame");
            // We just sought so we must flush the codec buffers
            avcodec_flush_buffers(m_pCodecCtx);
            // decoding until getting the correct frame or end of stream
//            printf("sought to ts %ld, now decoding frame %lu at ts %ld\n", keyframeTs, frame, frameTs);
        }
        // fast forwarding to frame of interest
        for (;;) {
            decodeNextFrame();
            if (m_CurrentFrame == frame) return;
            if (m_CurrentFrame > frame) throw runtime_error("requested frame does not exist in stream, movie index looks corrupted");
        }
    }

    AVCodecContext * getCodecContextPtr() const {
        return m_pCodecCtx;
    }

private:
    const Stream& m_Stream;
    AVCodecContext *m_pCodecCtx;
    StreamPacketReader m_PacketReader;
    std::unique_ptr<AVFrame> m_pFrameHolder;
    size_t m_CurrentFrame;
};

struct PictureDecoder {
    PictureDecoder(AVCodecContext *pCodecCtx) :
                    width(pCodecCtx->width), height(pCodecCtx->height), m_pSwsCtx(nullptr) {
        // fetching scaling context
        const int scalingFlags = SWS_POINT;
        SwsFilter * const pSrcFilter = nullptr;
        SwsFilter * const pDstFilter = nullptr;
        const double * const pParams = nullptr;
        m_pSwsCtx = sws_getCachedContext(m_pSwsCtx, width, height, pCodecCtx->pix_fmt, width, height, PIX_FMT_RGB24, scalingFlags, pSrcFilter, pDstFilter, pParams);
        if (!m_pSwsCtx) {
            throw std::runtime_error("cannot get valid context for image decoding");
        }

        // sws_scale need lineStripe multiple of 8
        const auto multiple = 8;
        lineSize = width * 3;
        const auto remainder = lineSize % multiple;
        const bool contiguousMemory = remainder == 0;
        roundedUpLineSize = contiguousMemory ? lineSize : lineSize + multiple - remainder;

        m_Buffer.resize(lineSize * height);
        m_StridedBuffer.resize(roundedUpLineSize * height);
        for (int i = 0; i < AV_NUM_DATA_POINTERS; ++i)
            lineSizes[i] = roundedUpLineSize;
    }

    const uint8_t* decodeFrame(const AVFrame *pFrame) const {
        uint8_t* pSrc = m_StridedBuffer.data();
        if (sws_scale(m_pSwsCtx, pFrame->data, pFrame->linesize, 0, height, &pSrc, lineSizes) != pFrame->height) {
            throw std::runtime_error("cannot decode image");
        }
        if (m_Buffer.size() == m_StridedBuffer.size()) {
            return pSrc;
        }
        uint8_t* pDest = m_Buffer.data();
        for (int i = 0; i < height; ++i, pSrc += roundedUpLineSize, pDest += lineSize)
            memcpy(pDest, pSrc, lineSize);
        return m_Buffer.data();

    }

    void debugFrame(const AVFrame *pFrame, const char* filename) const {
        ofstream file;
        file.open(filename, ios::binary);
        file << "P6" << '\n';
        file << std::to_string(width) << ' ' << std::to_string(height) << '\n';
        file << "255\n";
        file.write(reinterpret_cast<const char*>(decodeFrame(pFrame)), m_Buffer.size());
        file.close();
        printf("wrote %s\n", filename);
    }
public:
    const int width, height;
private:
    int lineSize, roundedUpLineSize;
    struct SwsContext* m_pSwsCtx;
    mutable std::vector<uint8_t> m_StridedBuffer, m_Buffer;
    int lineSizes[AV_NUM_DATA_POINTERS];
};

std::once_flag gAvCodecInitializer;

struct LibAvInitializer {
    LibAvInitializer() {
        std::call_once(gAvCodecInitializer, []() {av_register_all();});
    }
};

}  // namespace

namespace duke {

class LibAVIOReader : public IImageReader {
private:
    LibAvInitializer m_LibAvInitialization;
    MovieContainer m_Container;
    Stream m_Stream;
    StreamFrameDecoder m_Decoder;
    PictureDecoder m_PictureDecoder;
    uint64_t m_FrameCount;

public:
    LibAVIOReader(const Attributes& options, const IIODescriptor *pDesc, const char *filename)
    try :
                    IImageReader(options, pDesc), m_LibAvInitialization(), m_Container(filename), m_Stream(m_Container), m_Decoder(m_Stream), m_PictureDecoder(
                                    m_Decoder.getCodecContextPtr()), m_FrameCount(m_Stream.getContainerIndex().getFrameCount()) {
        printf("found %lu frames...\n", m_FrameCount);
        m_ReaderAttributes.set<attribute::MediaFrameCount>(m_FrameCount);
        m_ReaderAttributes.set<attribute::OiioColorspace>("sRGB");
    }
    catch (const std::runtime_error &e) {
        m_Error = e.what();
    }

    virtual bool doSetup(PackedFrameDescription& description, Attributes& frameAttributes) override {
        try {
            const auto requestedFrame = frameAttributes.getOrDie<attribute::MediaFrame>();
            m_Decoder.decodeFrame(requestedFrame + m_Stream.getFirstFrame());
            description.width = m_PictureDecoder.width;
            description.height = m_PictureDecoder.height;
            description.dataSize = m_PictureDecoder.width * m_PictureDecoder.height * 3;
            description.glPackFormat = GL_RGB8;
            return true;
        } catch (const exception &e) {
            m_Error = e.what();
            return false;
        }
    }

    virtual const void* getMappedImageData() const override {
        return m_PictureDecoder.decodeFrame(m_Decoder.getCurrentFramePtr());
    }
};

class LibAVIODescriptor : public IIODescriptor {
public:
    LibAVIODescriptor() :
                    m_Extensions( { "mov", "mp4", "avi", "mxf", "mkv", "mpg" }) {
    }
    virtual bool supports(Capability capability) const override {
        return capability == Capability::READER_GENERAL_PURPOSE;
    }
    virtual const vector<string>& getSupportedExtensions() const override {
        return m_Extensions;
    }
    virtual const char* getName() const override {
        return "AVLibIO";
    }
    virtual IImageReader * getReaderFromFile(const Attributes& options, const char *filename) const override {
        return new LibAVIOReader(options, this, filename);
    }
private:
    const vector<string> m_Extensions;
};

namespace {
bool registrar = IODescriptors::instance().registerDescriptor(new LibAVIODescriptor());
}  // namespace

}

#endif // DUKE_LIBAV

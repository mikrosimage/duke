#include <duke/engine/streams/SingleFileStream.hpp>
#include <duke/engine/streams/FileSequenceStream.hpp>

#include <duke/attributes/AttributeKeys.hpp>
#include <duke/attributes/Attributes.hpp>
#include <duke/base/StringAppender.hpp>
#include <duke/filesystem/FsUtils.hpp>
#include <duke/imageio/ImageLoadUtils.hpp>
#include <duke/memory/Allocator.hpp>
#include <sequence/Item.hpp>

#include <set>

namespace duke {

namespace {

std::vector<IIODescriptor*> findIODescriptors(const sequence::Item& item) {
  const auto& filename = item.filename;
  const char* pExtension = fileExtension(filename.c_str());
  const auto& descriptors = IODescriptors::instance().findDescriptor(pExtension);
  return {begin(descriptors), end(descriptors)};
}

bool isFileSequenceReader(const IIODescriptor* pDescriptor) {
  return pDescriptor->supports(IIODescriptor::Capability::READER_FILE_SEQUENCE);
}

}  // namespace

FileSequenceStream::FileSequenceStream(const sequence::Item& item)
    : m_FrameStart(item.start), m_Padding(item.padding), m_Descriptors(findIODescriptors(item)) {
  CHECK(item.getType() == sequence::Item::PACKED);
  CHECK(std::all_of(begin(m_Descriptors), end(m_Descriptors), &isFileSequenceReader));
  const auto& filename = item.filename;
  const auto begin = filename.begin();
  auto firstSharpIndex = filename.find('#');
  m_Prefix = std::string(begin, begin + firstSharpIndex);
  auto lastSharpIndex = filename.rfind('#');
  m_Suffix = std::string(begin + lastSharpIndex + 1, filename.end());
  using namespace attribute;
  set<MediaFrameCount>(m_State, item.end - item.start + 1);
  m_OpenResult = process(0);
}

const ReadFrameResult& FileSequenceStream::getResult() const { return m_OpenResult; }

// Several threads will access this function at the same time.
ReadFrameResult FileSequenceStream::process(const size_t atFrame) const {
  BufferStringAppender<2048> buffer;
  const size_t frame = atFrame + m_FrameStart;
  const size_t paddingSize = m_Padding > 0 ? m_Padding : digits(frame);
  buffer.append(m_Prefix);
  appendPaddedFrameNumber(frame, paddingSize, buffer);
  buffer.append(m_Suffix);
  CHECK(!buffer.full()) << "filename too long";
  return duke::load(buffer.c_str());
}

SingleFileStream::SingleFileStream(const sequence::Item& item) : m_OpenResult(load(item.filename.c_str())) {
  using namespace attribute;
  if (!m_OpenResult) {
    set<Error>(m_State, m_OpenResult.error.c_str());
    return;
  }
  CHECK(m_OpenResult.reader);
  set<File>(m_State, item.filename.c_str());
  set<MediaFrameCount>(m_State, m_OpenResult.reader->getContainerDescription().frames);
}

const ReadFrameResult& SingleFileStream::getResult() const { return m_OpenResult; }

ReadFrameResult SingleFileStream::process(const size_t frame) const {
  if (frame == 0) return m_OpenResult;
  CHECK(m_OpenResult.reader);
  using namespace attribute;
  std::lock_guard<std::mutex> guard(m_Mutex);
  ReadFrameResult result;
  result.reader = m_OpenResult.reader;
  duke::loadImage(result, [frame](const ContainerDescription&) {
    ReadOptions options;
    options.frame = frame;
    return options;
  });
  return result;
}

bool SingleFileStream::isForwardOnly() const {
  using namespace attribute;
  return getWithDefault<MediaFrameCount>(m_State) > 1;
}

}  // namespace duke

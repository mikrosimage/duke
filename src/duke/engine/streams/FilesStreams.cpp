#include "duke/engine/streams/SingleFileStream.hpp"
#include "duke/engine/streams/FileSequenceStream.hpp"

#include <duke/attributes/AttributeKeys.hpp>
#include <duke/attributes/Attributes.hpp>
#include <duke/base/StringAppender.hpp>
#include <duke/engine/ImageLoadUtils.hpp>
#include <duke/filesystem/FsUtils.hpp>
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

std::unique_ptr<IImageReader> getFirstValidReader(const attribute::Attributes& options,
                                                  const std::vector<IIODescriptor*>& descriptors,
                                                  const char* filename) {
  std::unique_ptr<IImageReader> pCurrent;
  for (const auto* pDescriptor : descriptors) {
    pCurrent.reset(pDescriptor->createFileReader(options, filename));
    if (!pCurrent) {
      printf("Discarding '%s' reader, read from memory is not yet implemented\n", pDescriptor->getName());
      continue;
    }
    if (pCurrent->hasError() == false) return pCurrent;
  }
  return pCurrent;
}

BigAlignedBlock gBigAlignedMallocator;

void CopyFromVolatileDataPointer(FrameData& frame, const void* pVolatileData) {
  if (!frame.pData) {
    const size_t dataSize = frame.description.dataSize;
    frame.pData = make_shared_memory<char>(dataSize, gBigAlignedMallocator);
    memcpy(frame.pData.get(), pVolatileData, dataSize);
  }
}

}  // namespace

FileSequenceStream::FileSequenceStream(const attribute::Attributes& options, const sequence::Item& item)
    : m_FrameStart(item.start), m_Padding(item.padding), m_Descriptors(findIODescriptors(item)), m_Options(options) {
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
  // reading first frame to get metadata
  merge(process(0).readerAttributes, m_State);
}

// Several threads will access this function at the same time.
ReadFrameResult FileSequenceStream::process(const size_t atFrame) const {
  ReadFrameResult result;
  BufferStringAppender<2048> buffer;
  const size_t frame = atFrame + m_FrameStart;
  const size_t paddingSize = m_Padding > 0 ? m_Padding : digits(frame);
  buffer.append(m_Prefix);
  appendPaddedFrameNumber(frame, paddingSize, buffer);
  buffer.append(m_Suffix);
  CHECK(!buffer.full()) << "filename too long";
  attribute::set<attribute::File>(result.attributes(), buffer.c_str());
  return duke::load(m_Options, &CopyFromVolatileDataPointer, std::move(result));
}

SingleFileStream::SingleFileStream(const attribute::Attributes& options, const sequence::Item& item)
    : m_Filename(item.filename),
      m_Descriptors(findIODescriptors(item)),
      m_pImageReader(getFirstValidReader(options, m_Descriptors, m_Filename.c_str())) {
  using namespace attribute;
  if (!m_pImageReader) {
    std::string error = "No reader for '";
    error += m_Filename;
    error += "'";
    set<Error>(m_State, error.c_str());
    return;
  }
  set<File>(m_State, m_Filename.c_str());
  merge(m_pImageReader->getAttributes(), m_State);
  if (m_pImageReader->hasError()) {
    set<Error>(m_State, m_pImageReader->getError().c_str());
    m_pImageReader.reset();
    return;
  }
}

ReadFrameResult SingleFileStream::process(const size_t frame) const {
  using namespace attribute;
  ReadFrameResult result;
  if (!m_pImageReader) {
    result.error = getWithDefault<Error>(m_State, "Invalid reader state");
    return result;
  }
  set<File>(result.attributes(), m_Filename.c_str());
  set<MediaFrame>(result.attributes(), frame);
  CHECK(m_pImageReader);
  std::lock_guard<std::mutex> guard(m_Mutex);
  return duke::loadImage(m_pImageReader.get(), &CopyFromVolatileDataPointer, std::move(result));
}

bool SingleFileStream::isForwardOnly() const {
  using namespace attribute;
  return getWithDefault<MediaFrameCount>(m_State) > 1;
}

}  // namespace duke

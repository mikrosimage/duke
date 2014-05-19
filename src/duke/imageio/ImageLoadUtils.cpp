#include <duke/imageio/ImageLoadUtils.hpp>

#include <duke/attributes/Attributes.hpp>
#include <duke/attributes/AttributeKeys.hpp>
#include <duke/filesystem/FsUtils.hpp>
#include <duke/filesystem/MemoryMappedFile.hpp>
#include <duke/gl/GlUtils.hpp>
#include <duke/gl/Textures.hpp>
#include <duke/image/ImageDescription.hpp>
#include <duke/image/ImageUtils.hpp>
#include <duke/imageio/DukeIO.hpp>
#include <duke/memory/Allocator.hpp>

#include <sstream>

using std::move;

namespace duke {

namespace {

AlignedMalloc alignedMalloc;

ReadFrameResult error(const std::string& error, ReadFrameResult& result) {
  result.error = error;
  return move(result);
}

}  // namespace

void loadImage(ReadFrameResult& result, const ReadOptionsFunc& getReadOptions) {
  IImageReader* pReader = result.reader.get();
  CHECK(pReader);
  if (pReader->hasError()) {
    result.error = pReader->getError();
    return;
  }
  const auto& description = pReader->getContainerDescription();
  CHECK(description.subimages.size() == 1);
  const auto& options = getReadOptions(description);
  if (!pReader->read(options, alignedMalloc, result.frame)) {
    result.error = pReader->getError();
    return;
  }
  // deducing other properties
  result.frame.updateOpenGlFormat();
}

ReadFrameResult load(const char* pFilename, const ReadOptionsFunc& getReadOptions) {
  ReadFrameResult result;
  if (!pFilename) return error("no filename", result);
  const char* pExtension = fileExtension(pFilename);
  if (!pExtension) return error("no extension", result);
  const auto& descriptors = IODescriptors::instance().findDescriptor(pExtension);
  if (descriptors.empty()) return error("no reader available", result);
  std::vector<std::string> errors;
  for (const IIODescriptor* pDescriptor : descriptors) {
    result.reader.reset(pDescriptor->createFileReader(pFilename));
    result.error.clear();
    loadImage(result, getReadOptions);
    if (result) return move(result);
    errors.emplace_back(pDescriptor->getName());
    errors.back() += " : ";
    errors.back() += result.error;
    errors.back() += "\n";
  }
  std::string msg("No reader succeeded for '");
  msg += pFilename;
  msg += "'\n";
  for (const auto& error : errors) msg += error;
  return error(msg, result);
}

} /* namespace duke */

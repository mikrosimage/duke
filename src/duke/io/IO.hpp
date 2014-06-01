/**
 * This file describes Duke's IO API.
 */

#pragma once

#include "duke/base/Check.hpp"
#include "duke/base/NonCopyable.hpp"
#include "duke/base/StringUtils.hpp"
#include "duke/image/FrameData.hpp"
#include "duke/image/ImageDescription.hpp"

#include <deque>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <cstddef>

namespace duke {

struct ContainerDescription {
  // Number of frames in this container. Will be one for single image.
  // Note: frame refers to temporal event, a frame can still have multiple layers
  // or exist in different streams.
  uint32_t frames = 0;

  // Describes available images :
  // - for single image format : only one ImageDescription.
  // - for multi part image format : one ImageDescription for every sub-image.
  // - for movies : one ImageDescription for every available streams.
  std::vector<ImageDescription> subimages;

  // All the extra informations available in the container.
  // eg. For a movie this could be the framerate, copyright informations, etc...
  attribute::Attributes metadata;
};

struct ReadOptions {
  uint32_t frame = 0;    // Index of frame to read.
  uint8_t subimage = 0;  // Index of image to read if multipart image.

  // In case you're interested in a sub range of channels, specify first and last
  // channels to consider.
  // eg. use [0,2] if images is RGBA but you only want RGB.
  int8_t channelRange[2] = {-1, -1};

  attribute::Attributes extra_attributes;  // Additional attributes.
};

/**
 * Interface for an image reader plugin.
 */
class IImageReader : public noncopyable {
 protected:
  std::string m_Error;
  ContainerDescription m_Description;

  bool error(const std::string& msg) {
    m_Error = msg;
    return false;
  }

 public:
  virtual ~IImageReader() {}

  inline bool hasError() const { return !m_Error.empty(); }

  inline std::string getError() const { return m_Error; }

  inline void clearError() { m_Error.clear(); }

  // Fill in a description of what's available.
  // Result will be meaningful only if no error.
  inline const ContainerDescription& getContainerDescription() const { return m_Description; }

  // Reads the specified image into data.
  // Returns false if reader is in invalid state. If so check error function above.
  virtual bool read(const ReadOptions& options, const Allocator& allocator, FrameData& frame) = 0;
};

/**
 * Interface to describe and instantiate a plugin.
 */
class IIODescriptor : public noncopyable {
 public:
  enum class Capability {
    READER_GENERAL_PURPOSE,  // Plugin can read several formats
    READER_FILE_SEQUENCE,    // Plugin will be instantiated for each frame, read will be parallel and out of order
  };
  virtual ~IIODescriptor() {}

  virtual const std::vector<std::string>& getSupportedExtensions() const = 0;

  virtual const char* getName() const = 0;

  virtual bool supports(Capability capability) const = 0;

  virtual IImageReader* createFileReader(const char* filename) const = 0;
};

/**
 * Class to hold the descriptors for all IO plugins.
 * This is the entry point for plugin retrieval.
 */
class IODescriptors : public noncopyable {
  std::vector<std::unique_ptr<IIODescriptor> > m_Descriptors;
  std::map<std::string, std::deque<IIODescriptor*>, ci_less> m_ExtensionToDescriptors;

 public:
  bool registerDescriptor(IIODescriptor* pDescriptor);

  const std::deque<IIODescriptor*>& findDescriptor(const char* extension) const;

  bool isSupported(const char* extension) const;

  inline const std::vector<std::unique_ptr<IIODescriptor> >& getDescriptors() const { return m_Descriptors; }

  static IODescriptors& instance();
};

}  // namespace duke

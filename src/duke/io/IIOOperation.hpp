#pragma once

#include <duke/image/FrameData.hpp>
#include <duke/io/IO.hpp>

#include <memory>

namespace duke {

/**
 * The result of an IO operation
 * - 'status' is one of SUCCESS, FAILURE.
 *   We could use other status for continuation if the process is segmented
 * - 'error' gives a message in case of failure.
 * - 'warning' gives a message if needed.
 */
struct IOResult {
  std::string error;
  operator bool() const { return error.empty(); }
};

/**
 * The result of an operation fetching an image
 */
struct ReadFrameResult : public IOResult {
  FrameData frame;
  std::shared_ptr<IImageReader> reader;
};

}  // namespace duke

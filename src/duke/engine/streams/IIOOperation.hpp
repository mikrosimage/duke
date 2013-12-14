#pragma once

#include <duke/base/NonCopyable.hpp>
#include <duke/imageio/RawPackedFrame.hpp>

namespace duke {

/**
 * The result of an IO operation
 * - 'status' is one of SUCCESS, FAILURE.
 *   We could use other status for continuation if the process is segmented
 * - 'error' gives a message in case of failure.
 * - 'warning' gives a message if needed.
 */
struct IOOperationResult : public noncopyable {
    enum Status { SUCCESS, FAILURE };

    Status status = FAILURE;
    std::string error;
    std::string warning;

    IOOperationResult() = default;
    IOOperationResult(IOOperationResult&&) = default;
    IOOperationResult& operator=(IOOperationResult&&) = default;

    operator bool() const {
        return status == SUCCESS;
    }
};

/**
 * The result of an operation fetching an image
 */
struct InputFrameOperationResult : public IOOperationResult {
    RawPackedFrame rawPackedFrame;

    InputFrameOperationResult() = default;
    InputFrameOperationResult(InputFrameOperationResult&&) = default;
    InputFrameOperationResult& operator=(InputFrameOperationResult&&) = default;

    inline Attributes& attributes() { return rawPackedFrame.attributes; }
    inline const Attributes& attributes() const { return rawPackedFrame.attributes; }
};

}  // namespace duke

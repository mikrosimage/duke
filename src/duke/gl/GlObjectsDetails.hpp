#pragma once

#include <cassert>

namespace duke {
namespace gl {

template <class GLOBJECT>
struct _Unbinder {
  void operator()(GLOBJECT *ptr) const {
    ptr->bind_count--;
    if (ptr->bind_count == 0) ptr->unbind();
  }
};

template <class GLOBJECT>
using _PTR = std::unique_ptr<GLOBJECT, _Unbinder<GLOBJECT> >;

template <class GLOBJECT>
struct Binder : public _PTR<GLOBJECT> {
  Binder(const GLOBJECT *ptr) : _PTR<GLOBJECT>(const_cast<GLOBJECT *>(ptr)) {
    assert(ptr);
    if (ptr->bind_count == 0) ptr->bind();
    ptr->bind_count++;
  }
  Binder(Binder &&other) = default;
  Binder() = default;
};

} /* namespace gl */
} /* namespace duke */

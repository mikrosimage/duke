#include "duke/base/StringSlice.hpp"

std::ostream& operator<<(std::ostream& stream, const StringSlice& slice) { return stream << slice.ToString(); }

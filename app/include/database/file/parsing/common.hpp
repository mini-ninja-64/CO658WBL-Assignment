#pragma once

#include <cstddef>
#include "utils/templating.hpp"

namespace common {
    template<typename T>
    constexpr size_t fixedLengthInBytesImplementation(TypeTag<T>);
};

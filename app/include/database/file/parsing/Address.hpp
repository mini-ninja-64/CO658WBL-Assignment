#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include <filesystem>

#include "utils/bitwise.hpp"
#include "utils/templating.hpp"
#include "deserialize.hpp"
#include "serialize.hpp"

namespace deserialize {
    template<>
    constexpr size_t fixedLengthInBytesImplementation(TypeTag<uint32_t>) { return 32/8; }

    template<>
    uint32_t fixedLengthTypeImplementation(FixedLengthDataBuffer<uint32_t> buffer, TypeTag<uint32_t>) {
        return UINT8_TO_UINT32(buffer, 0);
    }
}

namespace serialize {
    template<>
    constexpr size_t fixedLengthInBytesImplementation(TypeTag<uint32_t>) { return 32/8; }

    template<>
    FixedLengthDataBuffer<uint32_t> fixedLengthTypeImplementation(const uint32_t& element, TypeTag<uint32_t>) {
        return { UINT32_TO_UINT8(element) };
    }
}
#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include <filesystem>

#include "utils/bitwise.hpp"
#include "utils/templating.hpp"
#include "common.hpp"

template<>
struct Deserialize<uint32_t> {
    FIXED_LENGTH_DESERIALIZER(uint32_t, (32/8)) {
        return UINT8_TO_UINT32(it, 0);
    }
};

template<>
struct Serialize<uint32_t> {
    FIXED_LENGTH_SERIALIZER(uint32_t, (32/8)) {
        return { UINT32_TO_UINT8(it) };
    }
};

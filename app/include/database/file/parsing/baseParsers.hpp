#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>

#include "common.hpp"
#include "utils/bitwise.hpp"
#include "utils/templating.hpp"

template <> struct Deserialize<uint32_t> {
  FIXED_LENGTH_DESERIALIZER(uint32_t, (32 / 8)) {
    return UINT8_TO_UINT32(it, 0);
  }
};

template <> struct Serialize<uint32_t> {
  FIXED_LENGTH_SERIALIZER(uint32_t, (32 / 8)) { return {UINT32_TO_UINT8(it)}; }
};

template <> struct Serialize<boost::uuids::uuid> {
  FIXED_LENGTH_SERIALIZER(boost::uuids::uuid, 16) {
    std::array<uint8_t, 16> uuidBuffer{0};
    std::memcpy(uuidBuffer.data(), it.data, 16);
    return uuidBuffer;
  }
};

template <> struct Deserialize<boost::uuids::uuid> {
  FIXED_LENGTH_DESERIALIZER(boost::uuids::uuid, 16) {
    boost::uuids::uuid uuid{};
    std::memcpy(&uuid, it.data(), 16);

    return uuid;
  }
};

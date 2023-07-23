#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>

#include "database/file/parsing/common.hpp"
#include "utils/bitwise.hpp"

static constexpr size_t indexMetadataSize = (32 + 32) / 8;
template <typename ADDRESS> struct IndexMetadata {
  uint32_t graphOrder;
  uint32_t numberOfNodes;
  ADDRESS rootNode;
};

template <typename ADDRESS> struct Deserialize<IndexMetadata<ADDRESS>> {
  FIXED_LENGTH_DESERIALIZER(IndexMetadata<ADDRESS>,
                            4 + 4 + Deserialize<ADDRESS>::length) {
    return {.graphOrder = UINT8_TO_UINT32(it, 0),
            .numberOfNodes = UINT8_TO_UINT32(it, 4),
            .rootNode = Deserialize<ADDRESS>::fromBytes(
                it.template subspan<8, Deserialize<ADDRESS>::length>())};
  }
};

template <typename ADDRESS> struct Serialize<IndexMetadata<ADDRESS>> {
  FIXED_LENGTH_SERIALIZER(IndexMetadata<ADDRESS>,
                          4 + 4 + Deserialize<ADDRESS>::length) {
    std::array<uint8_t, Serialize<IndexMetadata<ADDRESS>>::length> buffer = {0};
    UINT32_TO_UINT8_ARRAY(buffer, 0, it.graphOrder);
    UINT32_TO_UINT8_ARRAY(buffer, 4, it.numberOfNodes);
    auto address = Serialize<ADDRESS>::toBytes(it.rootNode);
    std::copy(address.begin(), address.end(), buffer.begin() + 8);
    return std::move(buffer);
  }
};

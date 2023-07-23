#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>

#include "database/file/parsing/common.hpp"
#include "utils/bitwise.hpp"

static constexpr size_t dataMetadataSize = (32 + 32) / 8;
struct DataMetadata {
  uint32_t numberOfDataChunks;
  uint32_t dataChunkSize;
};

template <> struct Deserialize<DataMetadata> {
  FIXED_LENGTH_DESERIALIZER(DataMetadata, dataMetadataSize) {
    return {UINT8_TO_UINT32(it, 0), UINT8_TO_UINT32(it, 0)};
  }
};

template <> struct Serialize<DataMetadata> {
  FIXED_LENGTH_SERIALIZER(DataMetadata, dataMetadataSize) {
    return {UINT32_TO_UINT8(it.numberOfDataChunks),
            UINT32_TO_UINT8(it.dataChunkSize)};
  }
};

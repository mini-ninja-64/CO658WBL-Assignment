#pragma once

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>

#include "database/file/parsing/common.hpp"
#include "utils/bitwise.hpp"

constexpr size_t databaseFileHeaderSize = (32 + 8) / 8;
struct DatabaseFileHeader {
  uint32_t magicNumber;
  uint8_t formatVersion;
};

template <> struct Deserialize<DatabaseFileHeader> {
  FIXED_LENGTH_DESERIALIZER(DatabaseFileHeader, databaseFileHeaderSize) {
    const auto magicNumber = UINT8_TO_UINT32(it, 0);
    const auto formatVersion = it[4];
    if (formatVersion != 1)
      throw std::domain_error("Unsupported database file version");

    return {magicNumber, formatVersion};
  }
};

template <> struct Serialize<DatabaseFileHeader> {
  FIXED_LENGTH_SERIALIZER(DatabaseFileHeader, databaseFileHeaderSize) {
    constexpr auto sizeInBytes = Serialize<DatabaseFileHeader>::length;

    std::array<uint8_t, sizeInBytes> headerBuffer = {
        UINT32_TO_UINT8(it.magicNumber), it.formatVersion};

    return headerBuffer;
  }
};

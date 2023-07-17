#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include <filesystem>

#include "database/file/parsing/deserialize.hpp"
#include "database/file/parsing/serialize.hpp"
#include "utils/bitwise.hpp"

constexpr size_t databaseFileHeaderSize = (32 + 8)/8;
struct DatabaseFileHeader {
    uint32_t magicNumber;
    uint8_t formatVersion;
};

namespace deserialize {
    template<>
    constexpr size_t fixedLengthInBytes<DatabaseFileHeader>() {
        return databaseFileHeaderSize;
    }

    template<>
    DatabaseFileHeader fixedLengthType(FixedLengthDataBuffer<DatabaseFileHeader> buffer) {
        const auto magicNumber = UINT8_TO_UINT32(buffer,0);
        const auto formatVersion = buffer[4];
        if(formatVersion != 1) throw std::domain_error("Unsupported database file version");

        return {
            magicNumber,
            formatVersion
        };
    }
}

namespace serialize {
    template<>
    constexpr size_t fixedLengthInBytes<DatabaseFileHeader>() {
        return databaseFileHeaderSize;
    }

    template<>
    FixedLengthDataBuffer<DatabaseFileHeader> fixedLengthType(const DatabaseFileHeader& element) {
        constexpr auto sizeInBytes = fixedLengthInBytes<DatabaseFileHeader>();

        std::array<uint8_t, sizeInBytes> headerBuffer = {
                UINT32_TO_UINT8(element.magicNumber),
                element.formatVersion
        };

        return headerBuffer;
    }
}


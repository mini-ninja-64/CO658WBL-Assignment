#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include <filesystem>

#include "database/file/parsing/deserialize.hpp"
#include "database/file/parsing/serialize.hpp"
#include "utils/bitwise.hpp"

static constexpr size_t dataMetadataSize = (32)/8;
struct DataMetadata {
    uint32_t numberOfDataChunks;
};

namespace deserialize {
    template<>
    constexpr size_t fixedLengthInBytes<DataMetadata>() {
        return dataMetadataSize;
    }

    template<>
    DataMetadata fixedLengthType(FixedLengthDataBuffer<DataMetadata> buffer) {
        return {
            UINT8_TO_UINT32(buffer,0)
        };
    }
}

namespace serialize {
    template<>
    constexpr size_t fixedLengthInBytes<DataMetadata>() {
        return dataMetadataSize;
    }

    template<>
    FixedLengthDataBuffer<DataMetadata> fixedLengthType(const DataMetadata& element) {
        return {
            UINT32_TO_UINT8(element.numberOfDataChunks)
        };
    }
}


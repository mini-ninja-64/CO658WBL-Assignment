#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include <filesystem>

#include "database/file/parsing/deserialize.hpp"
#include "database/file/parsing/serialize.hpp"
#include "utils/bitwise.hpp"

static constexpr size_t indexMetadataSize = (32 + 32)/8;
struct IndexMetadata {
    uint32_t graphOrder;
    uint32_t numberOfNodes;
};

namespace deserialize {
    template<>
    constexpr size_t fixedLengthInBytes<IndexMetadata>() {
        return indexMetadataSize;
    }

    template<>
    IndexMetadata fixedLengthType(FixedLengthDataBuffer<IndexMetadata> buffer) {
        return {
            UINT8_TO_UINT32(buffer,0),
            UINT8_TO_UINT32(buffer,4)
        };
    }
}

namespace serialize {
    template<>
    constexpr size_t fixedLengthInBytes<IndexMetadata>() {
        return indexMetadataSize;
    }

    template<>
    FixedLengthDataBuffer<IndexMetadata> fixedLengthType(const IndexMetadata& element) {
        return {
            UINT32_TO_UINT8(element.graphOrder),
            UINT32_TO_UINT8(element.numberOfNodes)
        };
    }
}


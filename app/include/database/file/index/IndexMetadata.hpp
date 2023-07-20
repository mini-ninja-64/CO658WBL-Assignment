#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include <filesystem>

#include "database/file/parsing/common.hpp"
#include "utils/bitwise.hpp"

static constexpr size_t indexMetadataSize = (32 + 32)/8;
struct IndexMetadata {
    uint32_t graphOrder;
    uint32_t numberOfNodes;
};

template<>
struct Deserialize<IndexMetadata> {
    FIXED_LENGTH_DESERIALIZER(IndexMetadata, indexMetadataSize) {
        return {
                UINT8_TO_UINT32(it, 0),
                UINT8_TO_UINT32(it, 4)
        };
    }
};

template<>
struct Serialize<IndexMetadata> {
    FIXED_LENGTH_SERIALIZER(IndexMetadata, indexMetadataSize) {
        return {
            UINT32_TO_UINT8(it.graphOrder),
            UINT32_TO_UINT8(it.graphOrder)
        };
    }
};

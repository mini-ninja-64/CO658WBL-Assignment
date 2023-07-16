#pragma once

#include <cstdint>

static const auto GRAPH_DATABASE_BPLUS_METADATA_MINIMUM_SIZE = (32 + 32) / 8;
struct GraphDatabaseBPlusMetadata {
    uint32_t graphOrder;
    uint32_t graphNodeCount;
};

static const auto GRAPH_DATABASE_DATA_METADATA_MINIMUM_SIZE = (32) / 8;
struct GraphDatabaseDataMetadata {
    uint32_t dataChunkCount;
    uint32_t* dataLookupArray;
};


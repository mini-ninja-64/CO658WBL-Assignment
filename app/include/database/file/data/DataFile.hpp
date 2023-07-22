#pragma once

#include "database/file/Header.hpp"
#include "database/file/PullThroughCache.hpp"
#include "database/file/parsing/common.hpp"
#include "DataMetadata.hpp"
#include "DataChunk.hpp"

template<typename ADDRESS>
class DataFile {
public:
    using DataChunkPointer = std::shared_ptr<DataChunk>;
private:
    std::fstream file;
    DataMetadata metadata;
    PullThroughCache<ADDRESS, DataChunkPointer, 100> cache;

    DataChunkPointer pullDataChunkFromFile(ADDRESS address) {
        auto node = Deserialize<DataChunk>::fromStream(address, file);
        return std::make_shared<DataChunk>(node);
    }

public:
    static const uint32_t MAGIC_NUMBER = 0x64617461;

    DataFile(const std::filesystem::path& filePath, bool forceOverwrite):
    metadata({
        .numberOfDataChunks = 0
    }),
    cache([this](auto address) -> auto { return this->pullDataChunkFromFile(address); }){
        const bool write = !exists(filePath) || forceOverwrite;

        auto streamConfig = std::ios::in | std::ios::out | std::ios::binary;
        if(forceOverwrite) streamConfig |= std::ios::trunc;

        file = std::fstream(filePath, streamConfig);

        if (write) {
            //
            DatabaseFileHeader header = { MAGIC_NUMBER, 1 };
            auto pointer = Serialize<DatabaseFileHeader>::toStream(header, 0, file);
            Serialize<DataMetadata>::toStream(metadata, pointer, file);
        } else {
            auto indexHeader = Deserialize<DatabaseFileHeader>::fromStream(0, file);
            if(indexHeader.magicNumber != MAGIC_NUMBER)
                throw std::domain_error("provided index file has incorrect magic number");

            // TODO: std::streampos smaller than size_t (complier/system/arch dependent)
            auto metadataPosition = Deserialize<DatabaseFileHeader>::length;
            metadata = Deserialize<DataMetadata>::fromStream(metadataPosition, file);
        }
    }

    std::shared_ptr<DataChunk> getData(ADDRESS address) {
        return cache.fetch(address);
    }
};

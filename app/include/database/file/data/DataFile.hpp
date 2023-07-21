#pragma once

#include "database/file/tree/LazyNode.hpp"
#include "database/file/Header.hpp"
#include "DataMetadata.hpp"

template<typename K, typename ADDRESS>
class DataFile {
private:
    std::fstream file;
    DataMetadata metadata;

public:
    static const uint32_t MAGIC_NUMBER = 0x64617461;

    DataFile(const std::filesystem::path& filePath, bool forceOverwrite):
    metadata({
        .numberOfDataChunks = 0
    }) {
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

//    ADDRESS insertNode(const FileBackedNode<K, ADDRESS>& node) {}
//
//    void updateNode(ADDRESS address, const FileBackedNode<K, ADDRESS>& node) {}
//
//    std::shared_ptr<FileBackedNode<K, ADDRESS>> readNode(ADDRESS address) {}
};

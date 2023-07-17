#pragma once

#include "database/file/LazyNode.hpp"
#include "database/file/Header.hpp"
#include "IndexMetadata.hpp"

template<typename K, typename ADDRESS>
class IndexFile {
private:
    std::fstream file;
    std::unique_ptr<FileBackedNode<K, ADDRESS>> root;
    IndexMetadata metadata;
    std::streampos insertionPosition;

    void writeMetadata() {
        constexpr auto metadataPosition = serialize::fixedLengthInBytes<IndexMetadata>();
        serialize::toStream(metadata, metadataPosition, file);
    }

public:
    static const uint32_t MAGIC_NUMBER = 0x696E6478;

    IndexFile(const std::filesystem::path& filePath, bool forceOverwrite, uint32_t defaultOrder) {
        const bool write = !exists(filePath) || forceOverwrite;

        auto streamConfig = std::ios::in | std::ios::out | std::ios::binary;
        if(forceOverwrite) streamConfig |= std::ios::trunc;

        file = std::fstream(filePath, streamConfig);

        if (write) {
            //
            DatabaseFileHeader indexHeader = { MAGIC_NUMBER, 1 };
            auto indexPointer = serialize::toStream(indexHeader, 0, file);
            metadata = {
                    .graphOrder = defaultOrder,
                    .numberOfNodes = 0,
            };
            serialize::toStream(metadata, indexPointer, file);
        } else {
            auto indexHeader = deserialize::fromStream<DatabaseFileHeader>(0, file);
            if(indexHeader.magicNumber != MAGIC_NUMBER)
                throw std::domain_error("provided index file has incorrect magic number");

            // TODO: std::streampos smaller than size_t (complier/system/arch dependent)
            auto indexMetadataPosition = deserialize::fixedLengthInBytes<DatabaseFileHeader>();
            metadata = deserialize::fromStream<IndexMetadata>(indexMetadataPosition, file);
        }

        file.seekg(0, std::ios_base::end);
        insertionPosition = file.tellg();

//        if (metadata.numberOfNodes == 0) {
//
//        }
    }

    ADDRESS insertNode(const FileBackedNode<K, ADDRESS>& node) {
        ADDRESS newNodeAddress = insertionPosition;
        insertionPosition = serialize::toStream(node, newNodeAddress, file);

        metadata.numberOfNodes++;
        writeMetadata();

        return newNodeAddress;
    }
//
//    void updateNode(ADDRESS address, const FileBackedNode<K, ADDRESS>& node) {}
//
//    std::shared_ptr<FileBackedNode<K, ADDRESS>> readNode(ADDRESS address) {}
};

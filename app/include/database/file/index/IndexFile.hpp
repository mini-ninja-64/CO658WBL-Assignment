#pragma once

#include "database/file/Header.hpp"
#include "IndexMetadata.hpp"
#include "database/file/tree/FileBackedNode.hpp"
#include "database/file/tree/FileBackedInternal.hpp"
#include "database/file/tree/FileBackedLeaf.hpp"
#include "database/file/parsing/serialize.hpp"

template<typename K, typename ADDRESS>
class IndexFile {
private:
    std::fstream file;
    IndexMetadata metadata;
    std::streampos insertionPosition;

    void writeMetadata() {
        constexpr auto metadataPosition = serialize::fixedLengthInBytes<IndexMetadata>();
        serialize::toStream(metadata, metadataPosition, file);
    }

public:
    static const uint32_t MAGIC_NUMBER = 0x696E6478;

    IndexFile(const std::filesystem::path& filePath, bool forceOverwrite, uint32_t defaultOrder):
    metadata({
        .graphOrder = defaultOrder,
        .numberOfNodes = 0
    }) {
        const bool write = !exists(filePath) || forceOverwrite;

        auto streamConfig = std::ios::in | std::ios::out | std::ios::binary;
        if(forceOverwrite) streamConfig |= std::ios::trunc;

        file = std::fstream(filePath, streamConfig);

        if (write) {
            //
            DatabaseFileHeader indexHeader = { MAGIC_NUMBER, 1 };
            auto indexPointer = serialize::toStream(indexHeader, 0, file);
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

        if (metadata.numberOfNodes == 0) {
            newNode(NodeType::Leaf);
        }
    }

    using NodePointer = std::shared_ptr<FileBackedNode<K, ADDRESS>>;
    NodePointer newNode([[maybe_unused]] NodeType type) {
        ADDRESS newNodeAddress = insertionPosition;

        NodePointer node;
        switch (type) {
            case NodeType::Internal:
                //TODO: make internal node
                node = std::dynamic_pointer_cast<FileBackedNode<K, ADDRESS>>(
                        std::make_shared<FileBackedLeaf<K, ADDRESS>>(*this, newNodeAddress)
                );
                break;

            case NodeType::Leaf:
                node = std::dynamic_pointer_cast<FileBackedNode<K, ADDRESS>>(
                        std::make_shared<FileBackedLeaf<K, ADDRESS>>(*this, newNodeAddress)
                );
                break;
        }
        std::cout << serialize::fixedLengthInBytesImplementation(TypeTag<FileBackedNode<K, ADDRESS>>{}) << std::endl;
        std::cout << serialize::fixedLengthInBytes<FileBackedNode<K, ADDRESS>>() << std::endl;
//        insertionPosition = serialize::toStream(*node, newNodeAddress, file);
//
//        metadata.numberOfNodes++;
//        writeMetadata();

        return nullptr;
    }

    std::shared_ptr<FileBackedNode<K, ADDRESS>> getNode(ADDRESS address) {
        return std::make_shared(deserialize::fromStream(address, file));
    }

    void writeNode(ADDRESS address, const FileBackedNode<K, ADDRESS>& node) {
        serialize::toStream(node, address, file);
    }

//
//    void updateNode(ADDRESS address, const FileBackedNode<K, ADDRESS>& node) {}
//
//    std::shared_ptr<FileBackedNode<K, ADDRESS>> readNode(ADDRESS address) {}
};

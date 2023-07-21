#pragma once

#include "database/file/Header.hpp"
#include "IndexMetadata.hpp"
#include "database/file/tree/parser.hpp"
#include "database/file/tree/FileBackedNode.hpp"
#include "database/file/tree/FileBackedInternal.hpp"
#include "database/file/tree/FileBackedLeaf.hpp"

template<typename K, typename ADDRESS>
class IndexFile {
private:
    std::fstream file;
    IndexMetadata metadata;
    std::streampos insertionPosition;

    void writeMetadata() {
        constexpr auto metadataPosition = Serialize<DatabaseFileHeader>::length;
        Serialize<IndexMetadata>::toStream(metadata, metadataPosition, file);
    }

public:
    static const uint32_t MAGIC_NUMBER = 0x696E6478;

    IndexFile(const std::filesystem::path& filePath, bool forceOverwrite, uint32_t defaultOrder):
    metadata({ .graphOrder = defaultOrder, .numberOfNodes = 0 }) {
        const bool write = !exists(filePath) || forceOverwrite;

        auto streamConfig = std::ios::in | std::ios::out | std::ios::binary;
        if(forceOverwrite) streamConfig |= std::ios::trunc;

        file = std::fstream(filePath, streamConfig);

        if (write) {
            DatabaseFileHeader indexHeader = { MAGIC_NUMBER, 1 };
            auto indexPointer = Serialize<DatabaseFileHeader>::toStream(indexHeader, 0, file);
            Serialize<IndexMetadata>::toStream(metadata, indexPointer, file);
        } else {
            auto indexHeader = Deserialize<DatabaseFileHeader>::fromStream(0, file);
            if(indexHeader.magicNumber != MAGIC_NUMBER)
                throw std::domain_error("provided index file has incorrect magic number");

            // TODO: std::streampos smaller than size_t (complier/system/arch dependent)
            auto indexMetadataPosition = Deserialize<DatabaseFileHeader>::length;
            metadata = Deserialize<IndexMetadata>::fromStream(indexMetadataPosition, file);
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
        insertionPosition = Serialize<FileBackedNode<K, ADDRESS>, GraphContext>::toStream(*node, insertionPosition, file, {
                .order = metadata.graphOrder
            });

        metadata.numberOfNodes++;
        writeMetadata();
        return nullptr;
    }

//    std::shared_ptr<FileBackedNode<K, ADDRESS>> getNode(ADDRESS address) {
//        return std::make_shared(Deserialize<FileBackedNode<K, ADDRESS>>::fromStream(address, file));
//    }
//
//    void writeNode(ADDRESS address, const FileBackedNode<K, ADDRESS>& node) {
//        Serialize<FileBackedNode<K, ADDRESS>>::toStream(node, address, file);
//    }

//
//    void updateNode(ADDRESS address, const FileBackedNode<K, ADDRESS>& node) {}
//
//    std::shared_ptr<FileBackedNode<K, ADDRESS>> readNode(ADDRESS address) {}
};

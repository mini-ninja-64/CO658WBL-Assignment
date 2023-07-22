#pragma once

#include "database/file/Header.hpp"
#include "IndexMetadata.hpp"
#include "database/file/index/tree/nodeParser.hpp"
#include "database/file/index/tree/FileBackedNode.hpp"
#include "database/file/index/tree/FileBackedInternal.hpp"
#include "database/file/index/tree/FileBackedLeaf.hpp"
#include "database/file/PullThroughCache.hpp"

template<typename K, typename ADDRESS>
class IndexFile {
public:
    using NodePointer = std::shared_ptr<FileBackedNode<K, ADDRESS>>;
private:
    std::fstream file;
    IndexMetadata metadata;
    std::streampos insertionPosition;
    PullThroughCache<ADDRESS, NodePointer> cache;

    void writeMetadata() {
        constexpr auto metadataPosition = Serialize<DatabaseFileHeader>::length;
        Serialize<IndexMetadata>::toStream(metadata, metadataPosition, file);
    }

    NodePointer pullNodeFromFile(ADDRESS address) {
        auto node = Deserialize<std::unique_ptr<FileBackedNode<K, ADDRESS>>, DeserializeGraphContext<K,ADDRESS>>::fromStream(address, file, {
                .indexFile = *this,
                .order = metadata.graphOrder
        });
        // Convert our initially loaded unique smart pointer,
        // to a shared smart pointer for storage in the cache
        return std::move(node);
    }

public:
    static const uint32_t MAGIC_NUMBER = 0x696E6478;

    IndexFile(const std::filesystem::path& filePath, bool forceOverwrite, uint32_t defaultOrder):
        metadata({ .graphOrder = defaultOrder, .numberOfNodes = 0 }),
        cache([this](auto address) -> auto { return this->pullNodeFromFile(address); }, 100) {
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
        if (metadata.numberOfNodes == 0) insertNode(std::make_shared<FileBackedLeaf<K, ADDRESS>>(FileBackedLeaf<K, ADDRESS>(*this, {0,1,2,3}, {0,1,2,3})));
    }

    LazyNode<K, ADDRESS> insertNode(NodePointer node) {
        const auto nodeAddress =  static_cast<ADDRESS>(insertionPosition);
        insertionPosition = Serialize<FileBackedNode<K, ADDRESS>, SerializeGraphContext>::toStream(*node, nodeAddress, file, {
                .order = metadata.graphOrder
        });
        cache.populate(nodeAddress, node);

        metadata.numberOfNodes++;
        writeMetadata();
        return {*this, nodeAddress, node};
    }

    void saveNode(ADDRESS address, NodePointer node) {
        Serialize<FileBackedNode<K, ADDRESS>, SerializeGraphContext>::toStream(*node, address, file, {
                .order = metadata.graphOrder
        });
        cache.populate(address, node);
    }

    NodePointer getNode(ADDRESS address) {
        return cache.fetch(address);
    }

    const ADDRESS &getNextNodeAddress() const {
        // TODO: error check int overflow
        return static_cast<ADDRESS>(insertionPosition);
    }

    LazyNode<K,ADDRESS> getRootNode() {
        constexpr auto rootAddress = Serialize<DatabaseFileHeader>::length + Serialize<IndexMetadata>::length;
        return {*this, rootAddress};
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

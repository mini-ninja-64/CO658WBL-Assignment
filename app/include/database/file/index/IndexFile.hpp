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
    IndexMetadata<ADDRESS> metadata;
    std::streampos insertionPosition;
    PullThroughCache<ADDRESS, NodePointer, 100> cache;

    void writeMetadata() {
        constexpr auto metadataPosition = Serialize<DatabaseFileHeader>::length;
        Serialize<IndexMetadata<ADDRESS>>::toStream(metadata, metadataPosition, file);
    }

    NodePointer pullNodeFromFile(ADDRESS address) {
        auto node = Deserialize<std::unique_ptr<FileBackedNode<K, ADDRESS>>, DeserializeGraphContext<K,ADDRESS>>::fromStream(address, file, {
                .indexFile = this,
                .order = metadata.graphOrder
        });
        // Convert our initially loaded unique smart pointer,
        // to a shared smart pointer for storage in the cache
        return std::move(node);
    }

public:
    static constexpr uint32_t MAGIC_NUMBER = 0x696E6478;

    IndexFile(const std::filesystem::path& filePath, bool forceOverwrite, uint32_t defaultOrder):
        metadata({
            .graphOrder = defaultOrder,
            .numberOfNodes = 0,
            // By default, the root node will be the first node after the metadata
            .rootNode = Serialize<DatabaseFileHeader>::length + Serialize<IndexMetadata<ADDRESS>>::length
        }),
        cache([this](auto address) -> auto { return this->pullNodeFromFile(address); }) {
        const bool write = !exists(filePath) || forceOverwrite;

        auto streamConfig = std::ios::in | std::ios::out | std::ios::binary;
        if(forceOverwrite) streamConfig |= std::ios::trunc;

        file = std::fstream(filePath, streamConfig);

        if (write) {
            DatabaseFileHeader indexHeader = { MAGIC_NUMBER, 1 };
            auto indexPointer = Serialize<DatabaseFileHeader>::toStream(indexHeader, 0, file);
            Serialize<IndexMetadata<ADDRESS>>::toStream(metadata, indexPointer, file);
        } else {
            auto indexHeader = Deserialize<DatabaseFileHeader>::fromStream(0, file);
            if(indexHeader.magicNumber != MAGIC_NUMBER)
                throw std::domain_error("provided index file has incorrect magic number");

            // TODO: std::streampos smaller than size_t (complier/system/arch dependent)
            auto indexMetadataPosition = Deserialize<DatabaseFileHeader>::length;
            metadata = Deserialize<IndexMetadata<ADDRESS>>::fromStream(indexMetadataPosition, file);
        }

        file.seekg(0, std::ios_base::end);
        insertionPosition = file.tellg();
        if (metadata.numberOfNodes == 0)
            insertNode(std::make_shared<FileBackedLeaf<K, ADDRESS>>(this));
    }

    LazyNode<K, ADDRESS> insertNode(NodePointer node) {
        const auto nodeAddress =  static_cast<ADDRESS>(insertionPosition);
        insertionPosition = Serialize<FileBackedNode<K, ADDRESS>, SerializeGraphContext>::toStream(*node, nodeAddress, file, {
                .order = metadata.graphOrder
        });
        cache.populate(nodeAddress, node);

        metadata.numberOfNodes++;
        writeMetadata();
        return { this, nodeAddress };
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

    ADDRESS getNextNodeAddress() const {
        // TODO: error check int overflow
        return static_cast<ADDRESS>(insertionPosition);
    }

    LazyNode<K,ADDRESS> getRootNode() {
        return {this, metadata.rootNode};
    }

    const IndexMetadata<ADDRESS> &getMetadata() const {
        return metadata;
    }

    void setRootNode(ADDRESS address) {
        metadata.rootNode = address;
        writeMetadata();
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

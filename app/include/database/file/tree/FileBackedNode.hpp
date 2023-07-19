#pragma once

#include <vector>
#include "database/file/index/IndexFile.hpp"

enum class NodeType {
    Internal,
    Leaf
};

// Pre-declare required classes to prevent circular includes
template<typename K, typename ADDRESS>
class IndexFile;
template<typename K, typename ADDRESS>
class LazyNode;

template<typename K, typename ADDRESS>
class FileBackedNode {
private:
    IndexFile<K, ADDRESS>& indexFile;
    ADDRESS address;
    std::vector<K> records;

    std::optional<LazyNode<K, ADDRESS>> parent;

    void saveNodeToFile() {
        indexFile.writeNode(address, *this);
    }

public:
    FileBackedNode(IndexFile<K, ADDRESS>& indexFile, ADDRESS address, const std::vector<K> &records) :
        indexFile(indexFile),
        address(address),
        records(records),
        parent(std::nullopt) {}

    FileBackedNode(IndexFile<K, ADDRESS>& indexFile, ADDRESS address, const std::vector<K> &records, LazyNode<K, ADDRESS> parent) :
        indexFile(indexFile),
        address(address),
        records(records),
        parent(parent) {}

    FileBackedNode(IndexFile<K, ADDRESS>& indexFile, ADDRESS address, const std::vector<K> &records, ADDRESS parentAddress) :
        indexFile(indexFile),
        address(address),
        records(records),
        parent(indexFile, parentAddress) {}

    virtual ~FileBackedNode() {};

    virtual NodeType getNodeType() = 0;

    void setParent(ADDRESS newAddress) {
        parent = LazyNode<K, ADDRESS>(indexFile, newAddress);
        saveNodeToFile();
    }

    std::optional<LazyNode<K, ADDRESS>> getParent() {
        return parent;
    }
};

/*
 * | data              | size in bytes        |
 * |-------------------|----------------------|
 * | node type         | 1                    |
 * | number of records | 4                    |
 * | records           | type size * ORDER    |
 * | data addresses    | address size * ORDER |
 * | next leaf         | address size         |
 * | prev leaf         | address size         |
 */

//namespace common {
//    template<typename K>
//    constexpr size_t fixedLengthInBytesImplementation(TypeTag<FileBackedNode<K , uint32_t>>) { return 32/8; }
//}

namespace serialize {
    template<typename K, typename ADDRESS>
    constexpr size_t fixedLengthInBytesImplementation(TypeTag<FileBackedNode<K , ADDRESS>>) {
        return 0;
    }

    template<typename K, typename ADDRESS>
    FixedLengthDataBuffer<FileBackedNode<K, ADDRESS>> fixedLengthTypeImplementation(
            [[maybe_unused]] const FileBackedNode<K, ADDRESS> &element,
            TypeTag<FileBackedNode<K, ADDRESS>>) {
        return {};
    }
}

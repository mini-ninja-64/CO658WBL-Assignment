#pragma once

#include <vector>
#include <optional>

#include "database/file/parsing/common.hpp"
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
 *
 * A = Address Size
 * K = Key Size
 * O = Graph Order
 *
 * Internal Node Data Layout:
 * | data               | size in bytes              |
 * |--------------------|----------------------------|
 * | node type          | 1                          |
 * | parent             | A                          |
 * | number of records  | 4                          |
 * | records            | K * O                      |
 * | children addresses | A * (O + 1)                |
 * | reserved           | A                          |
 *
 * Leaf Data Layout:
 * | data               | size in bytes              |
 * |------------------- |----------------------------|
 * | node type          | 1                          |
 * | parent             | address size               |
 * | number of records  | 4                          |
 * | records            | K * O                      |
 * | data addresses     | A * O                      |
 * | next leaf          | A                          |
 * | prev leaf          | A                          |
 * Size: 1 + A + 4 + (K * O) + (A * O) + A + A
 *     : 5 + 3A + O(K + A)
 */


//template<typename K, typename ADDRESS>
//static size_t fileBackedNodeSize(size_t order) {
//    auto keySize = Deserialize<K>::size;
//    auto addressSize = Deserialize<ADDRESS>::size;
//    return 5 + (3 * addressSize) + (order *  (keySize + addressSize));
//}
//
//template<typename K, typename ADDRESS>
//struct Deserialize<FileBackedNode<K, ADDRESS>> {
//    static FileBackedNode<K, ADDRESS> fromStream(std::streampos position, std::fstream& fileStream) {
//        const auto size = fileBackedNodeSize<K, ADDRESS>(order);
//    }
//};
//
//template<typename K, typename ADDRESS>
//struct Serialize<FileBackedNode<K, ADDRESS>> {
//    static std::streampos toStream(const FileBackedNode<K, ADDRESS> &element, std::streampos position, std::fstream &fileStream) {
//        auto order = element.;
//        const auto size = fileBackedNodeSize<K, ADDRESS>(order);
//    }
//};

template<typename T, typename ADDRESS>
struct Deserialize<FileBackedNode<T, ADDRESS>> {
    using DeserializeType = FileBackedNode<T, ADDRESS>;
    FIXED_LENGTH_DESERIALIZER(DeserializeType, 1) {
        return {0};
    }
};

template<typename T, typename ADDRESS>
struct Serialize<FileBackedNode<T, ADDRESS>> {
    using SerializeType = FileBackedNode<T, ADDRESS>;
    FIXED_LENGTH_SERIALIZER(SerializeType, 1) {
        return {0};
    }
};

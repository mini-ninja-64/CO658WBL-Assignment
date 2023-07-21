#pragma once

#include <vector>
#include <optional>

#include "database/file/parsing/common.hpp"
#include "database/file/index/IndexFile.hpp"

#include "database/file/parsing/base_parsers.hpp"
#include "FileBackedLeaf.hpp"
#include "FileBackedInternal.hpp"
#include "types.hpp"

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

    std::optional<ADDRESS> parent;

    void saveNodeToFile() {
        indexFile.writeNode(address, *this);
    }

public:
    FileBackedNode(IndexFile<K, ADDRESS>& indexFile, ADDRESS address, const std::vector<K> &records) :
        indexFile(indexFile),
        address(address),
        records(records),
        parent(std::nullopt) {}

    FileBackedNode(IndexFile<K, ADDRESS>& indexFile, ADDRESS address, const std::vector<K> &records, ADDRESS parent) :
        indexFile(indexFile),
        address(address),
        records(records),
        parent(parent) {}

    virtual ~FileBackedNode() = default;

    [[nodiscard]] virtual NodeType getNodeType() const = 0;

    void setParent(ADDRESS newAddress) {
        parent = LazyNode<K, ADDRESS>(indexFile, newAddress);
        saveNodeToFile();
    }

    std::optional<LazyNode<K, ADDRESS>> getParent() const {
        if(parent) return LazyNode<K, ADDRESS>{indexFile, parent.value()};
        return std::nullopt;
    }

    const std::vector<K> &getRecords() const {
        return records;
    }

};

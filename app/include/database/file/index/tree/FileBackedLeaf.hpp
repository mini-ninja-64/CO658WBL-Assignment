#pragma once

#include <optional>

#include "FileBackedNode.hpp"
#include "LazyNode.hpp"
#include "types.hpp"

template<typename K, typename ADDRESS>
class FileBackedLeaf : public FileBackedNode<K, ADDRESS> {
private:
    std::vector<ADDRESS> dataAddresses;
    std::optional<ADDRESS> nextLeaf;
    std::optional<ADDRESS> previousLeaf;
public:
    explicit FileBackedLeaf(IndexFile<K, ADDRESS> &indexFile) :
        FileBackedNode<K, ADDRESS>(indexFile, {}),
        dataAddresses({}) {}

    FileBackedLeaf(IndexFile<K, ADDRESS> &indexFile,
                   const std::vector<K>& records,
                   const std::vector<ADDRESS>& dataAddresses) :
            FileBackedNode<K, ADDRESS>(indexFile, records),
            dataAddresses(dataAddresses) {}

    FileBackedLeaf(IndexFile<K, ADDRESS> &indexFile,
                   const std::vector<K>& records,
                   std::optional<ADDRESS> parent,
                   const std::vector<ADDRESS> &dataAddresses,
                   const std::optional<ADDRESS> &nextLeaf,
                   const std::optional<ADDRESS> &previousLeaf) :
                   FileBackedNode<K, ADDRESS>(indexFile, records, parent),
                   dataAddresses(dataAddresses),
                   nextLeaf(nextLeaf),
                   previousLeaf(previousLeaf) {}

    [[nodiscard]] const std::vector<ADDRESS> &getDataAddresses() const {
        return dataAddresses;
    }

    [[nodiscard]] virtual NodeType getNodeType() const {
        return NodeType::Leaf;
    }

    [[nodiscard]] std::optional<LazyNode<K,ADDRESS>> getNextNode() const {
        if(nextLeaf)
            return LazyNode<K, ADDRESS>(FileBackedNode<K, ADDRESS>::indexFile, nextLeaf.value());
        return std::nullopt;
    };

    [[nodiscard]] std::optional<LazyNode<K,ADDRESS>> getPreviousNode() const {
        if(previousLeaf)
            return LazyNode<K, ADDRESS>(FileBackedNode<K, ADDRESS>::indexFile, previousLeaf.value());
        return std::nullopt;
    };

//    void insertOrdered(const K& key, const V& value) {
//        auto insertIndex = this->insertableLocation(key);
//        this->records.insert(this->records.begin() + insertIndex, key);
//        values.insert(this->values.begin() + insertIndex, value);
//    }
//    std::optional<VALUE_TYPE> remove(const KEY_TYPE& key) {
//        auto recordIndex = this->indexOf(key);
//        if(!recordIndex.has_value()) return std::nullopt; // Key is not present, nothing to do except return early
//        auto deletedValue = values[recordIndex.value()];
//        this->records.erase(this->records.begin() + recordIndex.value());
//        this->values.erase(this->values.begin() + recordIndex.value());
//        return deletedValue;
//    }

//    /*
//     * Split the Leaf to the left
//     * creates and returns a new leaf which takes the records from the left hand side of the provided split index
//     * the existing leaf will be mutated
//     */
//    [[nodiscard]] Leaf<KEY_TYPE, VALUE_TYPE>* splitLeft(size_t splitIndex) {
//        auto newLeftLeaf = new Leaf<KEY_TYPE, VALUE_TYPE>(
//                {this->records.begin(), this->values.begin()+splitIndex},
//                {this->values.begin(), this->values.begin()+splitIndex}
//        );
//        this->records.erase(this->records.begin(), this->records.begin()+splitIndex);
//        this->values.erase(this->values.begin(), this->values.begin()+splitIndex);
//        if(this->previousLeaf != nullptr) {
//            newLeftLeaf->previousLeaf = this->previousLeaf;
//            this->previousLeaf->nextLeaf = newLeftLeaf;
//        }
//        newLeftLeaf->nextLeaf = this;
//        this->previousLeaf = newLeftLeaf;
//        return newLeftLeaf;
//    }
};

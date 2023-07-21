#pragma once

#include <optional>

#include "LazyNode.hpp"
#include "FileBackedNode.hpp"
#include "types.hpp"

template<typename K, typename ADDRESS>
class FileBackedLeaf : public FileBackedNode<K, ADDRESS> {
private:
    std::vector<ADDRESS> dataAddresses;
    ADDRESS nextNode;
    ADDRESS previousNode;
public:
    FileBackedLeaf(IndexFile<K, ADDRESS> &indexFile, ADDRESS address) :
        FileBackedNode<K, ADDRESS>(indexFile, address,{}),
        dataAddresses({}) {}

//    Internal(LazyChildNode leftChild, const KEY_TYPE& separator, LazyChildNode rightChild) :
//            Node<KEY_TYPE, VALUE_TYPE>({separator}),
//            children({leftChild, rightChild}) {
//        leftChild->setParent(this);
//        rightChild->setParent(this);
//    }
    [[nodiscard]] virtual NodeType getNodeType() const {
        return NodeType::Leaf;
    }

    [[nodiscard]] std::optional<LazyNode<K,ADDRESS>> getNextNode() const {
        // TODO: CachingNodeFactory lookup
        return std::nullopt;
    };

    [[nodiscard]] std::optional<LazyNode<K,ADDRESS>> getPreviousNode() const {
        // TODO: CachingNodeFactory lookup
        return std::nullopt;
    };
};
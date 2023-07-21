#pragma once

#include "LazyNode.hpp"

/*
 * Split the provided Leaf to the right
 * creates and returns a new leaf which takes the records from the right hand side of the provided split index
 * the existing leaf will be mutated
 */
template <typename K, typename ADDRESS>
[[nodiscard]] LazyNode<K, ADDRESS> splitRight(LazyNode<K, ADDRESS> leftNode, size_t splitIndex) {
    auto& indexFile = leftNode.get()->indexFile;
    std::vector<K> rightRecords = { leftNode.get()->records.begin() + splitIndex, leftNode.get()->records.end()  };

    std::shared_ptr<FileBackedNode<K, ADDRESS>> rightNodePointer;
    switch (leftNode.get()->getNodeType()) {
        case NodeType::Internal:
//            rightNodePointer = std::make_shared<FileBackedInternal<K, ADDRESS>>(
//                    leftLeaf->indexFile,
//                    rightRecords,
//                    {leftLeaf->dataAddresses.begin() + splitIndex, leftLeaf->dataAddresses.end()});
            break;
        case NodeType::Leaf: {
            auto leftLeaf = dynamic_cast<std::shared_ptr<FileBackedLeaf<K, ADDRESS>>>(leftNode.get());
            rightNodePointer = std::make_shared<FileBackedLeaf<K, ADDRESS>>(
                    indexFile,
                    rightRecords,
                    {leftLeaf->dataAddresses.begin() + splitIndex, leftLeaf->dataAddresses.end()});

            leftLeaf->dataAddresses.erase(leftLeaf->dataAddresses.begin() + splitIndex, leftLeaf->dataAddresses.end());

            rightNodePointer->nextLeaf = leftLeaf->nextLeaf;
            rightNodePointer->prevLeaf = leftNode.getAddress();
            leftLeaf->nextLeaf = indexFile.getNextNodeAddress();
            }
            break;
    }
    leftNode.get()->records.erase(leftNode.get()->records.begin() + splitIndex, leftNode.get()->records.end());
    rightNodePointer->parent = leftNode.get()->parent;

    indexFile.saveNode(leftNode.getAddress(), leftNode.get());
    return indexFile.insertNode(rightNodePointer);
}
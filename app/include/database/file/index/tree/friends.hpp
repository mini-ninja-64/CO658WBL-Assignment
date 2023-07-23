#pragma once

#include "LazyNode.hpp"

/*
 * Split the provided Leaf to the right
 * creates and returns a new leaf which takes the records from the right hand side of the provided split index
 * the existing leaf will be mutated
 */
template <typename K, typename ADDRESS>
[[nodiscard]] LazyNode<K, ADDRESS> splitRight(LazyNode<K, ADDRESS> leftLazyNode, size_t splitIndex) {
    auto leftNode = leftLazyNode.get();
    auto indexFile = leftNode->indexFile;
    std::vector<K> rightRecords = { leftNode->records.begin() + splitIndex, leftNode->records.end()  };

    std::shared_ptr<FileBackedNode<K, ADDRESS>> rightNodePointer;
    switch (leftNode->getNodeType()) {
        case NodeType::Internal: {
            auto leftInternal = std::static_pointer_cast<FileBackedInternal<K, ADDRESS>>(leftNode);
            auto rightChildren = std::vector<ADDRESS> {leftInternal->childrenAddresses.begin() + splitIndex + 1, leftInternal->childrenAddresses.end()};
            for (auto& childAddress : rightChildren) {
                auto lazyChild = LazyNode<K, ADDRESS> { indexFile, childAddress };
                lazyChild.get()->setParentAddress(indexFile->getNextNodeAddress());
            }
            rightNodePointer = std::make_shared<FileBackedInternal<K, ADDRESS>>(
                    indexFile,
                    std::vector<K> { leftNode->records.begin() + splitIndex + 1, leftNode->records.end() },
                    leftInternal->parent,
                    std::move(rightChildren)
                    );
            leftInternal->childrenAddresses.erase(leftInternal->childrenAddresses.begin() + splitIndex + 1, leftInternal->childrenAddresses.end());
        }
            break;
        case NodeType::Leaf: {
            auto leftLeaf = std::dynamic_pointer_cast<FileBackedLeaf<K, ADDRESS>>(leftNode);
            rightNodePointer = std::make_shared<FileBackedLeaf<K, ADDRESS>>(
                    indexFile,
                    rightRecords,
                    leftLeaf->parent,
                    std::vector<ADDRESS> {leftLeaf->dataAddresses.begin() + splitIndex, leftLeaf->dataAddresses.end()},
                    leftLeaf->nextLeaf,
                    leftLazyNode.getAddress()
                    );

            leftLeaf->dataAddresses.erase(leftLeaf->dataAddresses.begin() + splitIndex, leftLeaf->dataAddresses.end());
            leftLeaf->nextLeaf = indexFile->getNextNodeAddress();
            }
            break;
    }
    leftNode->records.erase(leftNode->records.begin() + splitIndex, leftNode->records.end());

    indexFile->saveNode(leftLazyNode.getAddress(), leftNode);
    return indexFile->insertNode(rightNodePointer);
}
#pragma once

#include "LazyNode.hpp"

template<typename KEY_TYPE, typename ADDRESS>
class FileBackedInternal : public FileBackedNode<KEY_TYPE, ADDRESS> {
    using LazyChildNode = LazyNode<FileBackedNode<KEY_TYPE, ADDRESS>, ADDRESS>;
private:
    std::vector<LazyChildNode> children;

    FileBackedInternal(const std::vector<KEY_TYPE>& records,
                      const std::vector<LazyChildNode>& children) :
            FileBackedNode<KEY_TYPE, ADDRESS>(records),
            children(children) {}
public:
//    FileBackedInternal(LazyChildNode leftChild, const KEY_TYPE& separator, LazyChildNode rightChild) :
//            Node<KEY_TYPE, VALUE_TYPE>({separator}),
//            children({leftChild, rightChild}) {
//        leftChild->setParent(this);
//        rightChild->setParent(this);
//    }
    virtual NodeType getNodeType() {
        return NodeType::Internal;
    }

    const std::vector<LazyChildNode>& getChildren() const {
        return children;
    }

//    // Find the next child based on the provided key
//    LazyChildNode next(const KEY_TYPE& key) {
//        auto nextIndex = this->insertableLocation(key);
//        return children[nextIndex];
//    }
//
//    void addChild(const KEY_TYPE& childKey, LazyChildNode childNode) {
//        auto insertIndex = this->insertableLocation(childKey);
//        this->records.insert(this->records.begin() + insertIndex, childKey);
//        this->children.insert(this->children.begin() + insertIndex + 1, childNode);
//        childNode->setParent(this);
//    }
    /*
     * Split the internal Node to the right
     * creates and returns a new leaf which takes the records from the right hand side of the provided split index
     * the existing leaf will be mutated
     */
//    [[nodiscard]] Internal<KEY_TYPE, VALUE_TYPE>* splitRight(size_t splitIndex) {
//        auto newRightInternal = new Internal<KEY_TYPE, VALUE_TYPE>(
//                {this->records.begin() + splitIndex+1, this->records.end()},
//                {this->children.begin() + splitIndex+1, this->children.end()}
//        );
//        this->records.erase(this->records.begin() + splitIndex, this->records.end());
//        this->children.erase(this->children.begin() + splitIndex + 1, this->children.end());
//        return newRightInternal;
//    }
    /*
     * Split the Leaf to the left
     * creates and returns a new leaf which takes the records from the left hand side of the provided split index
     * the existing leaf will be mutated
     */
//    [[nodiscard]] Internal<KEY_TYPE, VALUE_TYPE>* splitLeft(size_t splitIndex) {
//        auto newLeftInternal = new Internal<KEY_TYPE, VALUE_TYPE>(
//                {this->records.begin(), this->records.begin() + splitIndex},
//                {this->children.begin(), this->children.end()  + splitIndex + 1}
//        );
//        this->records.erase(this->records.begin(), this->records.begin() + splitIndex);
//        this->children.erase(this->children.begin(), this->children.begin() + splitIndex + 1);
//        return newLeftInternal;
//    }
//
//    void insertOrdered(KEY_TYPE key, VALUE_TYPE value) {
//        auto insertIndex = insertableLocation(key);
//        this->records.insert(records.begin() + insertIndex, key)
//        children.insert(children.begin() + insertIndex, key)
//    }
};
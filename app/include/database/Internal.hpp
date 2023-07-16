#pragma once

#include <cstdint>
#include <cstddef>

#include "database/Node.hpp"

template<size_t MAX_SIZE, typename KEY_TYPE, typename VALUE_TYPE>
class Internal : public Node2<MAX_SIZE, KEY_TYPE, VALUE_TYPE> {
    typedef Node2<MAX_SIZE, KEY_TYPE, VALUE_TYPE>* ChildNode;
private:
    std::vector<ChildNode> children;

    explicit Internal(const std::vector<KEY_TYPE>& records,
                      const std::vector<ChildNode>& children) :
                      Node2<MAX_SIZE, KEY_TYPE, VALUE_TYPE>(records),
                      children(children) {}

public:
    explicit Internal(ChildNode leftChild, const KEY_TYPE& separator, ChildNode rightChild) :
        Node2<MAX_SIZE, KEY_TYPE, VALUE_TYPE>({separator}),
        children({leftChild, rightChild}) {
        leftChild->setParent(this);
        rightChild->setParent(this);
    }

    virtual NodeType getNodeType() {
        return NodeType::Internal;
    }

    const std::vector<ChildNode> &getChildren() const {
        return children;
    }

    // Find the next child based on the provided key
    ChildNode next(const KEY_TYPE& key) {
        auto nextIndex = this->insertableLocation(key);
        return children[nextIndex];
    }

    void addChild(const KEY_TYPE& childKey, ChildNode childNode) {
        auto insertIndex = this->insertableLocation(childKey);
        this->records.insert(this->records.begin() + insertIndex, childKey);
        this->children.insert(this->children.begin() + insertIndex + 1, childNode);
        childNode->setParent(this);
    }

    /*
     * Split the internal Node to the right
     * creates and returns a new leaf which takes the records from the right hand side of the provided split index
     * the existing leaf will be mutated
     */
    [[nodiscard]] Internal<MAX_SIZE, KEY_TYPE, VALUE_TYPE>* splitRight(size_t splitIndex) {
        auto newRightInternal = new Internal<MAX_SIZE, KEY_TYPE, VALUE_TYPE>(
                {this->records.begin() + splitIndex+1, this->records.end()},
                {this->children.begin() + splitIndex+1, this->children.end()}
        );

        this->records.erase(this->records.begin() + splitIndex, this->records.end());
        this->children.erase(this->children.begin() + splitIndex + 1, this->children.end());

        return newRightInternal;
    }

    /*
     * Split the Leaf to the left
     * creates and returns a new leaf which takes the records from the left hand side of the provided split index
     * the existing leaf will be mutated
     */
    [[nodiscard]] Internal<MAX_SIZE, KEY_TYPE, VALUE_TYPE>* splitLeft(size_t splitIndex) {
        auto newLeftInternal = new Internal<MAX_SIZE, KEY_TYPE, VALUE_TYPE>(
                {this->records.begin(), this->records.begin() + splitIndex},
                {this->children.begin(), this->children.end()  + splitIndex + 1}
        );

        this->records.erase(this->records.begin(), this->records.begin() + splitIndex);
        this->children.erase(this->children.begin(), this->children.begin() + splitIndex + 1);

        return newLeftInternal;
    }
//
//    void insertOrdered(KEY_TYPE key, VALUE_TYPE value) {
//        auto insertIndex = insertableLocation(key);
//        this->records.insert(records.begin() + insertIndex, key)
//        children.insert(children.begin() + insertIndex, key)
//    }
};
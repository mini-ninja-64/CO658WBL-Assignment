#pragma once

#include "LazyNode.hpp"

template<typename K, typename ADDRESS>
class FileBackedLeaf : public FileBackedNode<K, ADDRESS> {
    using LazyChildNode = LazyNode<FileBackedNode<K, ADDRESS>, ADDRESS>;

private:
    std::vector<ADDRESS> dataValues;
public:
    FileBackedLeaf(IndexFile<K, ADDRESS> &indexFile, ADDRESS address) :
        FileBackedNode<K, ADDRESS>(indexFile, address,{}),
        dataValues({}) {}

//    Internal(LazyChildNode leftChild, const KEY_TYPE& separator, LazyChildNode rightChild) :
//            Node<KEY_TYPE, VALUE_TYPE>({separator}),
//            children({leftChild, rightChild}) {
//        leftChild->setParent(this);
//        rightChild->setParent(this);
//    }
    virtual NodeType getNodeType() {
        return NodeType::Leaf;
    }
};
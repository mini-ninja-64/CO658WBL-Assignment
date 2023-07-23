#pragma once

#include "LazyNode.hpp"
#include "types.hpp"

template <typename K, typename ADDRESS>
class FileBackedInternal : public FileBackedNode<K, ADDRESS> {
  template <typename K_FRIEND, typename ADDRESS_FRIEND>
  friend LazyNode<K_FRIEND, ADDRESS_FRIEND>
  splitRight(LazyNode<K_FRIEND, ADDRESS_FRIEND> leafToSplit, size_t splitIndex);

private:
  std::vector<ADDRESS> childrenAddresses;

public:
  explicit FileBackedInternal(IndexFile<K, ADDRESS> *indexFile)
      : FileBackedNode<K, ADDRESS>(indexFile, {}), childrenAddresses({}) {}

  FileBackedInternal(IndexFile<K, ADDRESS> *indexFile,
                     const std::vector<K> &records,
                     std::optional<ADDRESS> parent,
                     const std::vector<ADDRESS> &childrenAddresses)
      : FileBackedNode<K, ADDRESS>(indexFile, records, parent),
        childrenAddresses(childrenAddresses) {}

  FileBackedInternal(IndexFile<K, ADDRESS> *indexFile, ADDRESS leftChild,
                     const K &separator, ADDRESS rightChild)
      : FileBackedNode<K, ADDRESS>(indexFile, {separator}),
        childrenAddresses({leftChild, rightChild}) {}

  [[nodiscard]] virtual NodeType getNodeType() const {
    return NodeType::Internal;
  }

  [[nodiscard]] const std::vector<ADDRESS> &getChildrenAddresses() const {
    return childrenAddresses;
  }
  //
  const LazyNode<K, ADDRESS> getChild(size_t index) const {
    return {this->indexFile, childrenAddresses[index]};
  }

  // Find the next child based on the provided key
  LazyNode<K, ADDRESS> next(const K &key) {
    auto nextIndex = this->insertableLocation(key);
    return {this->indexFile, childrenAddresses[nextIndex]};
  }

  void addChild(const K &childKey, ADDRESS childAddress) {
    auto insertIndex = this->insertableLocation(childKey);
    this->records.insert(this->records.begin() + insertIndex, childKey);
    this->childrenAddresses.insert(
        this->childrenAddresses.begin() + insertIndex + 1, childAddress);
  }
  /*
   * Split the internal Node to the right
   * creates and returns a new leaf which takes the records from the right hand
   * side of the provided split index the existing leaf will be mutated
   */
  //    [[nodiscard]] Internal<KEY_TYPE, VALUE_TYPE>* splitRight(size_t
  //    splitIndex) {
  //        auto newRightInternal = new Internal<KEY_TYPE, VALUE_TYPE>(
  //                {this->records.begin() + splitIndex+1, this->records.end()},
  //                {this->children.begin() + splitIndex+1,
  //                this->children.end()}
  //        );
  //        this->records.erase(this->records.begin() + splitIndex,
  //        this->records.end()); this->children.erase(this->children.begin() +
  //        splitIndex + 1, this->children.end()); return newRightInternal;
  //    }
  /*
   * Split the Leaf to the left
   * creates and returns a new leaf which takes the records from the left hand
   * side of the provided split index the existing leaf will be mutated
   */
  //    [[nodiscard]] Internal<KEY_TYPE, VALUE_TYPE>* splitLeft(size_t
  //    splitIndex) {
  //        auto newLeftInternal = new Internal<KEY_TYPE, VALUE_TYPE>(
  //                {this->records.begin(), this->records.begin() + splitIndex},
  //                {this->children.begin(), this->children.end()  + splitIndex
  //                + 1}
  //        );
  //        this->records.erase(this->records.begin(), this->records.begin() +
  //        splitIndex); this->children.erase(this->children.begin(),
  //        this->children.begin() + splitIndex + 1); return newLeftInternal;
  //    }
  //
  //    void insertOrdered(KEY_TYPE key, VALUE_TYPE value) {
  //        auto insertIndex = insertableLocation(key);
  //        this->records.insert(records.begin() + insertIndex, key)
  //        children.insert(children.begin() + insertIndex, key)
  //    }
};
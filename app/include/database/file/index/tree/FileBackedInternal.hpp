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

  LazyNode<K, ADDRESS> getChild(size_t index) const {
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
};
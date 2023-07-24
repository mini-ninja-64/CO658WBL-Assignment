#pragma once

#include <optional>

#include "FileBackedNode.hpp"
#include "LazyNode.hpp"
#include "types.hpp"

template <typename K, typename ADDRESS>
class FileBackedLeaf : public FileBackedNode<K, ADDRESS> {
  template <typename K_FRIEND, typename ADDRESS_FRIEND>
  friend LazyNode<K_FRIEND, ADDRESS_FRIEND>
  splitRight(LazyNode<K_FRIEND, ADDRESS_FRIEND> leafToSplit, size_t splitIndex);

private:
  std::vector<ADDRESS> dataAddresses;
  std::optional<ADDRESS> nextLeaf;
  std::optional<ADDRESS> previousLeaf;

public:
  explicit FileBackedLeaf(IndexFile<K, ADDRESS> *indexFile)
      : FileBackedNode<K, ADDRESS>(indexFile, {}), dataAddresses({}) {}

  FileBackedLeaf(IndexFile<K, ADDRESS> *indexFile,
                 const std::vector<K> &records,
                 const std::vector<ADDRESS> &dataAddresses)
      : FileBackedNode<K, ADDRESS>(indexFile, records),
        dataAddresses(dataAddresses) {}

  FileBackedLeaf(IndexFile<K, ADDRESS> *indexFile,
                 const std::vector<K> &records, std::optional<ADDRESS> parent,
                 const std::vector<ADDRESS> &dataAddresses,
                 const std::optional<ADDRESS> &nextLeaf,
                 const std::optional<ADDRESS> &previousLeaf)
      : FileBackedNode<K, ADDRESS>(indexFile, records, parent),
        dataAddresses(dataAddresses), nextLeaf(nextLeaf),
        previousLeaf(previousLeaf) {}

  [[nodiscard]] const std::vector<ADDRESS> &getDataAddresses() const {
    return dataAddresses;
  }

  [[nodiscard]] virtual NodeType getNodeType() const { return NodeType::Leaf; }

  [[nodiscard]] std::optional<LazyNode<K, ADDRESS>> getNextNode() const {
    if (nextLeaf)
      return LazyNode<K, ADDRESS>(this->indexFile, nextLeaf.value());
    return std::nullopt;
  };

  [[nodiscard]] std::optional<LazyNode<K, ADDRESS>> getPreviousNode() const {
    if (previousLeaf)
      return LazyNode<K, ADDRESS>(FileBackedNode<K, ADDRESS>::indexFile,
                                  previousLeaf.value());
    return std::nullopt;
  };

  void insertOrdered(const K &key, const ADDRESS &value) {
    auto insertIndex = this->insertableLocation(key);
    this->records.insert(this->records.begin() + insertIndex, key);
    dataAddresses.insert(this->dataAddresses.begin() + insertIndex, value);
  }
};

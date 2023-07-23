#pragma once

#include <optional>
#include <vector>

#include "database/file/index/IndexFile.hpp"
#include "database/file/parsing/common.hpp"

#include "database/file/parsing/baseParsers.hpp"
#include "friends.hpp"
#include "types.hpp"

// Pre-declare required classes to prevent circular includes
template <typename K, typename ADDRESS> class IndexFile;
template <typename K, typename ADDRESS> class LazyNode;

template <typename K, typename ADDRESS> class FileBackedNode {
  template <typename K_FRIEND, typename ADDRESS_FRIEND>
  friend LazyNode<K_FRIEND, ADDRESS_FRIEND>
  splitRight(LazyNode<K_FRIEND, ADDRESS_FRIEND> leafToSplit, size_t splitIndex);
  //    friend LazyNode<K, ADDRESS> splitLeft(LazyNode<K, ADDRESS> leafToSplit,
  //    size_t splitIndex);
protected:
  IndexFile<K, ADDRESS> *indexFile;
  std::vector<K> records;

  std::optional<ADDRESS> parent;

  // For sufficiently small datasets binary search will actually be slower than
  // a brute force, because of this we can have a small optimisation for data
  // under a certain threshold this is of course highly dependent on the
  // computer.
#define BINARY_SEARCH_THRESHOLD 500
  std::optional<size_t> indexOf(const K &key, size_t &finalIndex) const {
    size_t lowerBound = 0;
    size_t upperBound = records.size() - 1;

    if (records.size() <= BINARY_SEARCH_THRESHOLD) {
      finalIndex = 0;
      while (finalIndex < records.size()) {
        const auto &record = records[finalIndex];
        if (record == key)
          return finalIndex;
        if (record > key)
          break;
        finalIndex++;
      }
      return std::nullopt;
    }

    while (lowerBound <= upperBound) {
      size_t midPoint = (upperBound + lowerBound) / 2;
      const auto &record = records[midPoint];
      if (record == key) {
        return midPoint;
      } else if (record > key) {
        upperBound = midPoint - 1;
      } else if (record < key) {
        lowerBound = midPoint + 1;
      }
    }
    finalIndex = lowerBound;
    return std::nullopt;
  }

  size_t insertableLocation(const K &key) const {
    size_t newIndex = 0;
    std::optional<size_t> exactMatch = indexOf(key, newIndex);
    // As B+ Trees are right biased, if an exact match was found
    // we should bias to the right as the right hand side child
    // contains the provided key.
    return exactMatch.value_or(newIndex - 1) + 1;
  }

public:
  FileBackedNode(IndexFile<K, ADDRESS> *indexFile,
                 const std::vector<K> &records)
      : indexFile(indexFile), records(records), parent(std::nullopt) {}

  FileBackedNode(IndexFile<K, ADDRESS> *indexFile,
                 const std::vector<K> &records, std::optional<ADDRESS> parent)
      : indexFile(indexFile), records(records), parent(parent) {}

  virtual ~FileBackedNode() = default;

  [[nodiscard]] virtual NodeType getNodeType() const = 0;

  std::optional<LazyNode<K, ADDRESS>> getParent() const {
    if (parent)
      return LazyNode<K, ADDRESS>{indexFile, parent.value()};
    return std::nullopt;
  }

  void setParentAddress(const std::optional<ADDRESS> &newParentAddress) {
    FileBackedNode::parent = newParentAddress;
  }

  const std::vector<K> &getRecords() const { return records; }

  std::optional<size_t> indexOf(const K &key) const {
    size_t index = 0;
    return indexOf(key, index);
  }
};

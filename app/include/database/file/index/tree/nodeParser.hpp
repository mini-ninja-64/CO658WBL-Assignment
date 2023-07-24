#pragma once

#include "database/file/parsing/baseParsers.hpp"
#include "database/file/parsing/common.hpp"
#include <cstring>

#include "FileBackedInternal.hpp"
#include "FileBackedLeaf.hpp"
#include "FileBackedNode.hpp"

/*
 *
 * A = Address Size
 * K = Key Size
 * O = Graph Order
 *
 * Internal Node Data Layout:
 * | data               | size in bytes              |
 * |--------------------|----------------------------|
 * | node type          | 1                          |
 * | parent             | A                          |
 * | number of records  | 4                          |
 * | records            | K * O                      |
 * | children addresses | A * (O + 1)                |
 * | reserved           | A                          |
 *
 * Leaf Data Layout:
 * | data               | size in bytes              |
 * |------------------- |----------------------------|
 * | node type          | 1                          |
 * | parent             | address size               |
 * | number of records  | 4                          |
 * | records            | K * O                      |
 * | data addresses     | A * O                      |
 * | next leaf          | A                          |
 * | prev leaf          | A                          |
 * Size: 1 + A + 4 + (K * O) + (A * O) + A + A
 *     : 5 + 3A + O(K + A)
 */

template <typename K, typename ADDRESS>
static size_t fileBackedNodeSize(size_t order) {
  auto keySize = Deserialize<K>::length;
  auto addressSize = Deserialize<ADDRESS>::length;
  return 5 + (3 * addressSize) + (order * (keySize + addressSize));
}

#define SPAN_FROM_BUFFER(TYPE, BUFFER, OFFSET)                                 \
  std::span<uint8_t, Deserialize<TYPE>::length>(BUFFER + OFFSET,               \
                                                Deserialize<TYPE>::length)

template <typename T>
static std::vector<T> deserializeElements(uint8_t *buffer, size_t offset,
                                          size_t elementCount) {
  constexpr auto typeLength = Deserialize<T>::length;
  std::vector<T> elements;
  for (size_t i = 0; i < elementCount; ++i) {
    std::span<uint8_t, typeLength> span((buffer + offset) + (i * typeLength),
                                        typeLength);
    auto element = Deserialize<T>::fromBytes(span);
    elements.push_back(element);
  }
  return elements;
}

template <typename T> std::optional<T> zeroIsOptionalEmpty(T value) {
  return value == 0 ? std::nullopt : std::optional<T>(value);
}

template <typename K, typename ADDRESS> struct DeserializeGraphContext {
  IndexFile<K, ADDRESS> *indexFile;
  size_t order;
};

template <typename K, typename ADDRESS>
struct Deserialize<std::unique_ptr<FileBackedNode<K, ADDRESS>>,
                   DeserializeGraphContext<K, ADDRESS>> {
  static std::unique_ptr<FileBackedNode<K, ADDRESS>>
  fromStream(std::streampos position, std::fstream &fileStream,
             DeserializeGraphContext<K, ADDRESS> context) {
    const auto addressLength = Deserialize<ADDRESS>::length;
    [[maybe_unused]] const auto keyLength = Deserialize<K>::length;
    const auto nodeSize = fileBackedNodeSize<K, ADDRESS>(context.order);

    auto buffer = new uint8_t[nodeSize];
    fileStream.seekg(position);
    fileStream.read(reinterpret_cast<char *>(buffer), nodeSize);

    auto bufferPosition = 0;

    // Parse NodeType and check for illegal values
    auto rawNodeType = buffer[bufferPosition];
    if (rawNodeType != static_cast<uint8_t>(NodeType::Internal) &&
        rawNodeType != static_cast<uint8_t>(NodeType::Leaf))
      throw std::domain_error("Unsupported node type found");
    auto nodeType = static_cast<NodeType>(rawNodeType);
    bufferPosition++;

    const auto parent = zeroIsOptionalEmpty(Deserialize<ADDRESS>::fromBytes(
        SPAN_FROM_BUFFER(ADDRESS, buffer, bufferPosition)));
    bufferPosition += addressLength;

    const auto recordCount = UINT8_TO_UINT32(buffer, bufferPosition);
    bufferPosition += 4;

    std::vector<K> records =
        deserializeElements<K>(buffer, bufferPosition, recordCount);
    bufferPosition += addressLength * context.order;

    std::unique_ptr<FileBackedNode<K, ADDRESS>> nodePointer;
    switch (nodeType) {
    case NodeType::Internal: {
      auto childrenAddresses =
          deserializeElements<ADDRESS>(buffer, bufferPosition, recordCount + 1);
      FileBackedInternal<K, ADDRESS> loadedNode(context.indexFile, records,
                                                parent, childrenAddresses);
      nodePointer =
          std::make_unique<FileBackedInternal<K, ADDRESS>>(loadedNode);
      break;
    }

    case NodeType::Leaf: {
      auto dataAddresses =
          deserializeElements<ADDRESS>(buffer, bufferPosition, recordCount);
      bufferPosition += addressLength * context.order;
      auto nextLeaf = zeroIsOptionalEmpty(Deserialize<ADDRESS>::fromBytes(
          SPAN_FROM_BUFFER(ADDRESS, buffer, bufferPosition)));
      bufferPosition += addressLength;
      auto previousLeaf = zeroIsOptionalEmpty(Deserialize<ADDRESS>::fromBytes(
          SPAN_FROM_BUFFER(ADDRESS, buffer, bufferPosition)));
      FileBackedLeaf<K, ADDRESS> loadedNode(context.indexFile, records, parent,
                                            dataAddresses, nextLeaf,
                                            previousLeaf);
      nodePointer = std::make_unique<FileBackedLeaf<K, ADDRESS>>(loadedNode);
      break;
    }
    }

    delete[] buffer;
    return nodePointer;
  }
};

template <typename T>
static size_t serializeFixedLengthElementToBuffer(std::span<uint8_t> &buffer,
                                                  size_t offset,
                                                  const T &element) {
  if (Deserialize<T>::length > buffer.size() - offset)
    throw std::out_of_range("buffer not large enough for provided element");
  auto serializedElement = Serialize<T>::toBytes(element);
  std::memcpy(buffer.data() + offset, serializedElement.data(),
              serializedElement.size());
  return offset + serializedElement.size();
}

template <typename T>
static size_t
serializeFixedLengthElementsToBuffer(std::span<uint8_t> &buffer, size_t offset,
                                     const std::vector<T> &elements,
                                     size_t chunkSize) {
  auto bufferPosition = offset;
  for (const auto &element : elements) {
    bufferPosition =
        serializeFixedLengthElementToBuffer(buffer, bufferPosition, element);
  }
  auto chunksToPad = chunkSize - elements.size();
  auto bytesToPad = chunksToPad * Deserialize<T>::length;
  std::memset(buffer.data() + bufferPosition, 0, bytesToPad);
  return bufferPosition + bytesToPad;
}

struct SerializeGraphContext {
  size_t order;
};

template <typename K, typename ADDRESS>
struct Serialize<FileBackedNode<K, ADDRESS>, SerializeGraphContext> {
  static std::streampos toStream(const FileBackedNode<K, ADDRESS> &node,
                                 std::streampos position,
                                 std::fstream &fileStream,
                                 SerializeGraphContext context) {
    auto order = context.order;
    const auto nodeSize = fileBackedNodeSize<K, ADDRESS>(order);

    auto buffer =
        new uint8_t[nodeSize]{0}; // Zero initialize the buffer to ensure
                                  // reserved/unused sections are cleared
    std::span<uint8_t> bufferSpan = {buffer, nodeSize};
    size_t bufferPosition = 0;

    // Write Node type
    buffer[bufferPosition] = static_cast<uint8_t>(node.getNodeType());
    bufferPosition++;

    std::optional<LazyNode<K, ADDRESS>> parent = node.getParent();
    if (parent) {
      bufferPosition = serializeFixedLengthElementToBuffer(
          bufferSpan, bufferPosition, parent.value().getAddress());
    } else {
      bufferPosition += Serialize<ADDRESS>::length;
    }

    // Write records
    // TODO: check for integer overflow
    bufferPosition = serializeFixedLengthElementToBuffer(
        bufferSpan, bufferPosition,
        static_cast<uint32_t>(node.getRecords().size()));
    bufferPosition = serializeFixedLengthElementsToBuffer(
        bufferSpan, bufferPosition, node.getRecords(), context.order);

    // Write data specific to node types
    switch (node.getNodeType()) {
    case NodeType::Leaf: {
      auto leafNode = static_cast<const FileBackedLeaf<K, ADDRESS> &>(node);
      // Write Leaf Data Addresses
      bufferPosition = serializeFixedLengthElementsToBuffer(
          bufferSpan, bufferPosition, leafNode.getDataAddresses(),
          context.order);

      auto nextNode = leafNode.getNextNode();
      if (nextNode)
        bufferPosition = serializeFixedLengthElementToBuffer(
            bufferSpan, bufferPosition, nextNode.value().getAddress());
      else
        bufferPosition += Serialize<ADDRESS>::length;

      auto previousNode = leafNode.getPreviousNode();
      if (previousNode)
        serializeFixedLengthElementToBuffer(bufferSpan, bufferPosition,
                                            previousNode.value().getAddress());
    } break;
    case NodeType::Internal: {
      [[maybe_unused]] auto internalNode =
          static_cast<const FileBackedInternal<K, ADDRESS> *>(&node);
      serializeFixedLengthElementsToBuffer(bufferSpan, bufferPosition,
                                           internalNode->getChildrenAddresses(),
                                           context.order + 1);
    } break;
    }

    fileStream.seekp(position);
    fileStream.write(reinterpret_cast<char *>(buffer), nodeSize);
    delete[] buffer;

    return position + std::streamoff(nodeSize);
  }
};

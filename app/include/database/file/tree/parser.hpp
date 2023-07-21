#pragma once

#include "database/file/parsing/common.hpp"
#include "database/file/parsing/base_parsers.hpp"

#include "FileBackedNode.hpp"
#include "FileBackedInternal.hpp"
#include "FileBackedLeaf.hpp"

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

template<typename K, typename ADDRESS>
static size_t fileBackedNodeSize(size_t order) {
    auto keySize = Deserialize<K>::length;
    auto addressSize = Deserialize<ADDRESS>::length;
    return 5 + (3 * addressSize) + (order *  (keySize + addressSize));
}

template<typename T>
static size_t serializeFixedLengthElementToBuffer(std::span<uint8_t>& buffer, size_t offset, const T& element) {
    if(Deserialize<T>::length > buffer.size() - offset) throw std::out_of_range("buffer not large enough for provided element");
    auto serializedElement = Serialize<T>::toBytes(element);
    std::memcpy(buffer.data() + offset, serializedElement.data(), serializedElement.size());
    return offset + serializedElement.size();
}

template<typename T>
static size_t serializeFixedLengthElementsToBuffer(std::span<uint8_t>& buffer, size_t offset, const std::vector<T>& elements, size_t chunkSize) {
    auto bufferPosition = offset;
    for (const auto &element: elements) {
        bufferPosition = serializeFixedLengthElementToBuffer(buffer, bufferPosition, element);
    }
    auto chunksToPad = chunkSize - elements.size();
    auto bytesToPad = chunksToPad * Deserialize<T>::length;
    std::memset(buffer.data() + offset, 0, bytesToPad);
    return bufferPosition + bytesToPad;
}

struct GraphContext {
    size_t order;
};

template<typename K, typename ADDRESS>
struct Deserialize<FileBackedNode<K, ADDRESS>, GraphContext> {
    static FileBackedNode<K, ADDRESS> fromStream([[maybe_unused]]  std::streampos position, [[maybe_unused]] std::fstream& fileStream, GraphContext context) {
        const auto size = fileBackedNodeSize<K, ADDRESS>(context.order);
    }
};

template<typename K, typename ADDRESS>
struct Serialize<FileBackedNode<K, ADDRESS>, GraphContext> {
    static std::streampos toStream(const FileBackedNode<K, ADDRESS> &node, std::streampos position, std::fstream &fileStream, GraphContext context) {
        auto order = context.order;
        const auto nodeSize = fileBackedNodeSize<K, ADDRESS>(order);

        auto buffer = new uint8_t[nodeSize]{0}; // Zero initialize the buffer to ensure reserved/unused sections are cleared
        std::span<uint8_t> bufferSpan = {buffer, nodeSize};
        size_t bufferPosition = 0;

        // Write Node type
        buffer[bufferPosition] = static_cast<uint8_t>(node.getNodeType());
        bufferPosition++;

        // Write parent address
        std::optional<LazyNode<K, ADDRESS>> parent = node.getParent();
        if (parent) {
            bufferPosition = serializeFixedLengthElementToBuffer(bufferSpan, bufferPosition, parent.value().getAddress());
        }

        // Write records
        // TODO: check for integer overflow
        bufferPosition = serializeFixedLengthElementToBuffer(bufferSpan, bufferPosition, static_cast<uint32_t>(node.getRecords().size()));
        bufferPosition = serializeFixedLengthElementsToBuffer(bufferSpan, bufferPosition, node.getRecords(), context.order);

        // Write data specific to node types
        switch (node.getNodeType()) {
            case NodeType::Leaf: {
                auto leafNode = static_cast<const FileBackedLeaf<K, ADDRESS>&>(node);
                // TODO: write value addresses, for now just write empty
                std::vector<ADDRESS> emptyVector;
                bufferPosition = serializeFixedLengthElementsToBuffer(bufferSpan, bufferPosition, emptyVector, context.order);

                auto nextNode = leafNode.getNextNode();
                if(nextNode)
                    bufferPosition = serializeFixedLengthElementToBuffer(bufferSpan, bufferPosition, nextNode.value().getAddress());
                else bufferPosition += Serialize<ADDRESS>::length;

                auto previousNode = leafNode.getPreviousNode();
                if(previousNode)
                    bufferPosition = serializeFixedLengthElementToBuffer(bufferSpan, bufferPosition, previousNode.value().getAddress());
                else bufferPosition += Serialize<ADDRESS>::length;
            }
                break;
            case NodeType::Internal: {
                [[maybe_unused]] auto internalNode = static_cast<const FileBackedInternal<K, ADDRESS>&>(node);
                // TODO: write value addresses, for now just write empty
                std::vector<ADDRESS> children;
                bufferPosition = serializeFixedLengthElementsToBuffer(bufferSpan, bufferPosition, children, context.order + 1);
                bufferPosition = serializeFixedLengthElementToBuffer(bufferSpan, bufferPosition, (uint32_t) 0);
            }
                break;
        }

        fileStream.seekp(position);
        fileStream.write(reinterpret_cast<char *>(buffer), nodeSize);
        delete[] buffer;

        return nodeSize;
    }
};

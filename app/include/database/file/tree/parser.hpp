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

#define SPAN_FROM_BUFFER(TYPE, BUFFER, OFFSET) std::span(BUFFER + OFFSET, Deserialize<TYPE>::length)

template<typename T>
std::vector<T> deserializeElements(uint8_t* buffer, size_t offset,  size_t elementCount) {
    constexpr auto typeLength = Deserialize<T>::length;
    std::vector<T> elements;
    for (size_t i = 0; i < elementCount; ++i) {
        std::span<uint8_t, typeLength> span((buffer + offset) + (i * typeLength), typeLength);
        auto element = Deserialize<T>::fromBytes(span);
        elements.push_back(element);
    }
    return elements;
}

template<typename K, typename ADDRESS>
struct DeserializeGraphContext {
    IndexFile<K, ADDRESS>& indexFile;
    size_t order;
};


template<typename K, typename ADDRESS>
struct Deserialize<std::unique_ptr<FileBackedNode<K, ADDRESS>>, DeserializeGraphContext<K, ADDRESS>> {
    static std::unique_ptr<FileBackedNode<K, ADDRESS>> fromStream(std::streampos position, std::fstream& fileStream, DeserializeGraphContext<K, ADDRESS> context) {
        const auto addressLength = Deserialize<ADDRESS>::length;
        [[maybe_unused]] const auto keyLength = Deserialize<K>::length;
        const auto nodeSize = fileBackedNodeSize<K, ADDRESS>(context.order);

        auto buffer = new uint8_t[nodeSize];
        fileStream.seekg(position);
        fileStream.read(reinterpret_cast<char *>(buffer), nodeSize);

        auto bufferPosition = 0;
//        NodeType nodeType;
//        switch (buffer[bufferPosition]) {
//            case static_cast<uint8_t>(NodeType::Internal):
//                nodeType = NodeType::Internal;
//                break;
//
//            case static_cast<uint8_t>(NodeType::Leaf):
//                nodeType = NodeType::Leaf;
//                break;
//            default:
//                throw std::domain_error("Unsupported node type found");
//        }
        bufferPosition++;

//        const auto parent = Deserialize<ADDRESS>::fromBytes(SPAN_FROM_BUFFER(ADDRESS, buffer, bufferPosition));
        bufferPosition += addressLength;

        const auto recordCount = UINT8_TO_UINT32(buffer, bufferPosition);
        bufferPosition += 4;

        std::vector<K> records = deserializeElements<K>(buffer, bufferPosition, recordCount);

//        switch(nodeType) {
//            case NodeType::Internal:
//                break;
//            case NodeType::Leaf:
//
//                break;
//        }

        delete[] buffer;

        return std::make_unique<FileBackedLeaf<K, ADDRESS>>(context.indexFile, position);
    }
};

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

struct SerializeGraphContext {
    size_t order;
};

template<typename K, typename ADDRESS>
struct Serialize<FileBackedNode<K, ADDRESS>, SerializeGraphContext> {
    static std::streampos toStream(const FileBackedNode<K, ADDRESS> &node, std::streampos position, std::fstream &fileStream, SerializeGraphContext context) {
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

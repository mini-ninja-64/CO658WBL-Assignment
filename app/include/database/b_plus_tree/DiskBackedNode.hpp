// #pragma once
//
// #include <cstring>
// #include <vector>
// #include <optional>
// #include <span>
// #include <stdexcept>
//
// #include "utils/bitwise.hpp"
// #include "Internal.hpp"
//
// template<typename KEY_TYPE, typename VALUE_TYPE>
// class DiskBackedNode {
// protected:
//     std::vector<KEY_TYPE> records;
//     Node<KEY_TYPE, VALUE_TYPE>* parent = nullptr;
//
//     explicit DiskBackedNode(const std::vector<KEY_TYPE> &records) :
//     records(records) {}
//
//     // technically could pre-allocate
//
//     // For sufficiently small datasets binary search will actually be slower
//     than a brute force,
//     // because of this we can have a small optimisation for data under a
//     certain threshold
//     // this is of course highly dependent on the computer.
// #define BINARY_SEARCH_THRESHOLD 500
//     std::optional<size_t> indexOf(const KEY_TYPE& key, size_t& finalIndex)
//     const {
//         size_t lowerBound = 0;
//         size_t upperBound = records.size() - 1;
//
//         if (records.size() <= BINARY_SEARCH_THRESHOLD) {
//             finalIndex = 0;
//             while(finalIndex < records.size()) {
//                 const auto& record = records[finalIndex];
//                 if (record == key) return finalIndex;
//                 if (record > key) break;
//                 finalIndex++;
//             }
//             return std::nullopt;
//         }
//
//         while (lowerBound <= upperBound) {
//             size_t midPoint = (upperBound + lowerBound) / 2;
//             const auto& record = records[midPoint];
//             if (record == key) {
//                 return midPoint;
//             } else if (record > key) {
//                 upperBound = midPoint - 1;
//             } else if (record < key) {
//                 lowerBound = midPoint + 1;
//             }
//         }
//         finalIndex = lowerBound;
//         return std::nullopt;
//     }
//
//     size_t insertableLocation(const KEY_TYPE &key) const {
//         size_t newIndex = 0;
//         std::optional<size_t> exactMatch = indexOf(key, newIndex);
//         // As B+ Trees are right biased, if an exact match was found
//         // we should bias to the right as the right hand side child
//         // contains the provided key.
//         return exactMatch.value_or(newIndex - 1) + 1;
//     }
//
// public:
//     virtual NodeType getNodeType() = 0;
//
//     std::optional<size_t> indexOf(const KEY_TYPE &key) const {
//         size_t index = 0;
//         return indexOf(key, index);
//     }
//
//     const std::vector<KEY_TYPE>& getRecords() const {
//         return records;
//     }
//
//     Node<KEY_TYPE, VALUE_TYPE>* getParent() const {
//         return parent;
//     }
//
//     void setParent(Node<KEY_TYPE, VALUE_TYPE>* newParent) {
//         this->parent = newParent;
//     }
// };
//
// template<typename KEY_TYPE>
// constexpr size_t deserializedKeySizeInBytes();
// template<typename KEY_TYPE>
// KEY_TYPE deserializeKey(std::span<uint8_t> buffer);
//
// #define NODE_POINTER_TYPE uint32_t
// constexpr size_t nodePointerSize = sizeof(NODE_POINTER_TYPE);
//
// template<typename KEY_TYPE>
// size_t sizeInBytesOfNode(size_t order) {
//     return  1                                               + /* node type */
//             4                                               + /* number of
//             records */ deserializedKeySizeInBytes<KEY_TYPE>() * order  + /*
//             records */ nodePointerSize * (order + 1)                   + /*
//             pointers to children */ nodePointerSize + /* previous leaf */
//             nodePointerSize;                                  /* next leaf */
// }
//
// template<typename KEY_TYPE, typename VALUE_TYPE>
// DiskBackedNode<KEY_TYPE, VALUE_TYPE>* fromBuffer(size_t order,
// std::span<uint8_t> buffer) {
//     const auto nodeSize = sizeInBytesOfNode<KEY_TYPE>(order);
//     if(buffer.size() < nodeSize)
//         throw std::length_error("provided buffer not large enough to contain
//         node");
//
//     auto nodeTypeOrdinal = buffer[0];
//     if(nodeTypeOrdinal != static_cast<uint8_t>(NodeType::Internal) &&
//         nodeTypeOrdinal != static_cast<uint8_t>(NodeType::Leaf))
//         throw std::domain_error("Unsupported node type provided");
//
//     auto nodeType = static_cast<NodeType>(nodeTypeOrdinal);
//     auto numberOfRecords = UINT8_TO_UINT32(buffer, 1);
//
//     auto records = new KEY_TYPE[numberOfRecords];
//     auto keySize = deserializedKeySizeInBytes<KEY_TYPE>();
//     auto bufferPosition = 5;
//     for (uint32_t i = 0; i < numberOfRecords; ++i) {
//         records[i] = deserializeNodeKey({buffer[bufferPosition], keySize});
//         bufferPosition += i * keySize;
//     }
//
//     auto pointers = new NODE_POINTER_TYPE[records + 2];
//     for (int i = 0; i < records + 2; ++i) {
//         pointers[i] = UINT8_TO_UINT32(buffer, bufferPosition);
//         bufferPosition += nodePointerSize;
//     }
//
//     switch (nodeType) {
//         case NodeType::Internal:
//             break;
//         case NodeType::Leaf:
//             break;
//     }
//
//     delete[] records;
//     delete[] pointers;
// }
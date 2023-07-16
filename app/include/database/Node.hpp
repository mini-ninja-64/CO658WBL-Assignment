#pragma once

enum class NodeType {
    Internal,
    Leaf
};

template<size_t MAX_SIZE, typename KEY_TYPE, typename VALUE_TYPE>
class Node2 {
protected:
    std::vector<KEY_TYPE> records;
    Node2<MAX_SIZE, KEY_TYPE, VALUE_TYPE>* parent = nullptr;

    explicit Node2(const std::vector<KEY_TYPE> &records) : records(records) {}

    // technically could pre-allocate

    // For sufficiently small datasets binary search will actually be slower than a brute force,
    // because of this we can have a small optimisation for data under a certain threshold
    // this is of course highly dependent on the computer.
#define BINARY_SEARCH_THRESHOLD 500
    std::optional<size_t> indexOf(const KEY_TYPE& key, size_t& finalIndex) const {
        size_t lowerBound = 0;
        size_t upperBound = records.size() - 1;

        if (records.size() <= BINARY_SEARCH_THRESHOLD) {
            finalIndex = 0;
            while(finalIndex < records.size()) {
                const auto& record = records[finalIndex];
                if (record == key) return finalIndex;
                if (record > key) break;
                finalIndex++;
            }
            return std::nullopt;
        }

        while (lowerBound <= upperBound) {
            size_t midPoint = (upperBound + lowerBound) / 2;
            const auto& record = records[midPoint];
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

    size_t insertableLocation(const KEY_TYPE &key) const {
        size_t newIndex = 0;
        std::optional<size_t> exactMatch = indexOf(key, newIndex);
        // As B+ Trees are right biased, if an exact match was found
        // we should bias to the right as the right hand side child
        // contains the provided key.
        return exactMatch.value_or(newIndex - 1) + 1;
    }

public:
    static const size_t maxSize = MAX_SIZE;

    virtual NodeType getNodeType() = 0;

    std::optional<size_t> indexOf(const KEY_TYPE &key) const {
        size_t index = 0;
        return indexOf(key, index);
    }

    const std::vector<KEY_TYPE>& getRecords() const {
        return records;
    }

    Node2<MAX_SIZE, KEY_TYPE, VALUE_TYPE>* getParent() const {
        return parent;
    }

    void setParent(Node2<MAX_SIZE, KEY_TYPE, VALUE_TYPE>* newParent) {
        this->parent = newParent;
    }
};
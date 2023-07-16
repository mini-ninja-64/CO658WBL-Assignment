#pragma once

#include <cstdint>
#include <cstddef>

template<typename KEY_TYPE, typename VALUE_TYPE>
class Leaf : public Node<KEY_TYPE, VALUE_TYPE> {
private:
    std::vector<VALUE_TYPE> values;

    Leaf<KEY_TYPE, VALUE_TYPE>* nextLeaf = nullptr;
    Leaf<KEY_TYPE, VALUE_TYPE>* previousLeaf = nullptr;


    explicit Leaf(const std::vector<KEY_TYPE> &records, const std::vector<VALUE_TYPE> &values) :
    Node<KEY_TYPE, VALUE_TYPE>(records),
    values(values) {}

public:
    Leaf(): Node<KEY_TYPE, VALUE_TYPE>({}) {};

    [[nodiscard]] virtual NodeType getNodeType() {
        return NodeType::Leaf;
    }

    [[nodiscard]] const std::vector<VALUE_TYPE> &getValues() const {
        return values;
    }

    void insertOrdered(const KEY_TYPE& key, const VALUE_TYPE& value) {
        auto insertIndex = this->insertableLocation(key);
        this->records.insert(this->records.begin() + insertIndex, key);
        values.insert(this->values.begin() + insertIndex, value);
    }

    std::optional<VALUE_TYPE> remove(const KEY_TYPE& key) {
        auto recordIndex = this->indexOf(key);
        if(!recordIndex.has_value()) return std::nullopt; // Key is not present, nothing to do except return early

        auto deletedValue = values[recordIndex.value()];
        this->records.erase(this->records.begin() + recordIndex.value());
        this->values.erase(this->values.begin() + recordIndex.value());

        return deletedValue;
    }

    /*
     * Split the Leaf to the right
     * creates and returns a new leaf which takes the records from the right hand side of the provided split index
     * the existing leaf will be mutated
     */
    [[nodiscard]] Leaf<KEY_TYPE, VALUE_TYPE>* splitRight(size_t splitIndex) {
        auto newRightLeaf = new Leaf<KEY_TYPE, VALUE_TYPE>(
                {this->records.begin() + splitIndex, this->records.end()},
                {this->values.begin() + splitIndex, this->values.end()}
        );

        this->records.erase(this->records.begin() + splitIndex, this->records.end());
        this->values.erase(this->values.begin() + splitIndex, this->values.end());

        if(this->nextLeaf != nullptr) {
            newRightLeaf->nextLeaf = this->nextLeaf;
            this->nextLeaf->previousLeaf = newRightLeaf;
        }

        newRightLeaf->previousLeaf = this;
        this->nextLeaf = newRightLeaf;

        return newRightLeaf;
    }

    /*
     * Split the Leaf to the left
     * creates and returns a new leaf which takes the records from the left hand side of the provided split index
     * the existing leaf will be mutated
     */
    [[nodiscard]] Leaf<KEY_TYPE, VALUE_TYPE>* splitLeft(size_t splitIndex) {
        auto newLeftLeaf = new Leaf<KEY_TYPE, VALUE_TYPE>(
                {this->records.begin(), this->values.begin()+splitIndex},
                {this->values.begin(), this->values.begin()+splitIndex}
        );

        this->records.erase(this->records.begin(), this->records.begin()+splitIndex);
        this->values.erase(this->values.begin(), this->values.begin()+splitIndex);

        if(this->previousLeaf != nullptr) {
            newLeftLeaf->previousLeaf = this->previousLeaf;
            this->previousLeaf->nextLeaf = newLeftLeaf;
        }

        newLeftLeaf->nextLeaf = this;
        this->previousLeaf = newLeftLeaf;

        return newLeftLeaf;
    }
};
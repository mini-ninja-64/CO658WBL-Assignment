#include <stdexcept>
#include "database/file/BPlusSection.hpp"
#include "utils/bitwise.hpp"



BPlusSection::BPlusSection(uint32_t sectionLength, uint32_t graphOrder, uint32_t graphNodeCount) : sectionLength(
        sectionLength), graphOrder(graphOrder), graphNodeCount(graphNodeCount) {}

BPlusSection BPlusSection::deserializeBuffer(const std::span<uint8_t> &buffer) {
    const auto sectionLength = UINT8_TO_UINT32(buffer, 0);
    if(buffer.size() < sectionLength) throw std::range_error("Buffer shorter than expected");

    const auto graphOrder = UINT8_TO_UINT32(buffer, 4);
    const auto graphNodeCount = UINT8_TO_UINT32(buffer, 8);



    return {sectionLength, graphOrder, graphNodeCount};
}

uint32_t BPlusSection::getSectionLength() const {
    return sectionLength;
}

uint32_t BPlusSection::getGraphOrder() const {
    return graphOrder;
}

uint32_t BPlusSection::getGraphNodeCount() const {
    return graphNodeCount;
}

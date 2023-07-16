#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include <vector>

class BPlusSection {
private:
    uint32_t sectionLength;
    uint32_t graphOrder;
    uint32_t graphNodeCount;

public:
    BPlusSection(uint32_t sectionLength, uint32_t graphOrder, uint32_t graphNodeCount);
    [[nodiscard]] static BPlusSection deserializeStream(std::streamoff position, std::fstream& fileStream);
    [[nodiscard]] static BPlusSection deserializeBuffer(const std::span<uint8_t>& buffer);
    [[nodiscard]] std::vector<uint8_t> serializeBuffer();

    [[nodiscard]] uint32_t getSectionLength() const;
    [[nodiscard]] uint32_t getGraphOrder() const;
    [[nodiscard]] uint32_t getGraphNodeCount() const;
};
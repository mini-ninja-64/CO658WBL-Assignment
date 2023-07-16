#pragma once

#include <cstdint>
#include <cstddef>
#include <span>
#include <filesystem>

class Header {
private:
    uint32_t magicNumber;
    uint8_t formatVersion;
    uint32_t BPlusSection;
    uint32_t dataSection;

    [[nodiscard]] static Header deserializeBuffer(const std::span<uint8_t>& buffer);

public:
    const static size_t sizeInBytes = (32 + 8 + 32 + 32) / 8;

    Header(uint32_t magicNumber, uint8_t formatVersion, uint32_t bPlusSection, uint32_t dataSection);
    [[nodiscard]] static Header deserializeStream(std::streamoff position, std::fstream& fileStream);
    [[nodiscard]] std::array<uint8_t, sizeInBytes> serializeBuffer();

    [[nodiscard]] uint32_t getMagicNumber() const;

    [[nodiscard]] uint8_t getFormatVersion() const;

    [[nodiscard]] uint32_t getBPlusSection() const;

    [[nodiscard]] uint32_t getDataSection() const;
};
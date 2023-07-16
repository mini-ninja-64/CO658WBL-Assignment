#include <stdexcept>
#include <fstream>
#include "database/file/Header.hpp"
#include "utils/bitwise.hpp"

Header::Header(uint32_t magicNumber, uint8_t formatVersion, uint32_t bPlusSection, uint32_t dataSection) : magicNumber(
        magicNumber), formatVersion(formatVersion), BPlusSection(bPlusSection), dataSection(dataSection) {}

Header Header::deserializeStream(std::streamoff position, std::fstream& fileStream) {
    std::array<uint8_t, Header::sizeInBytes> headerBuffer = {0};
    fileStream.seekg(position);
    fileStream.read(reinterpret_cast<char *>(headerBuffer.data()), Header::sizeInBytes);
    if(fileStream.gcount() != Header::sizeInBytes) throw std::range_error("Database file header ended unexpectedly");
    return deserializeBuffer(headerBuffer);
}

Header Header::deserializeBuffer(const std::span<uint8_t>& buffer) {
    if(buffer.size() < Header::sizeInBytes) throw std::range_error("Database file buffer is not long enough");

    return {
            UINT8_TO_UINT32(buffer,0),
            buffer[4],
            UINT8_TO_UINT32(buffer, 5),
            UINT8_TO_UINT32(buffer, 9)
    };
}

std::array<uint8_t, Header::sizeInBytes> Header::serializeBuffer() {
    return {
            UINT32_TO_UINT8(magicNumber),
            formatVersion,
            UINT32_TO_UINT8(BPlusSection),
            UINT32_TO_UINT8(dataSection)
    };
}

uint32_t Header::getMagicNumber() const {
    return magicNumber;
}

uint8_t Header::getFormatVersion() const {
    return formatVersion;
}

uint32_t Header::getBPlusSection() const {
    return BPlusSection;
}

uint32_t Header::getDataSection() const {
    return dataSection;
}






#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>

#include "database/file/parsing/common.hpp"

class DataChunk {
private:
    const size_t length;
    std::shared_ptr<uint8_t[]> data;
public:
    DataChunk(size_t length, std::unique_ptr<uint8_t[]> data);
    DataChunk(size_t length, std::shared_ptr<uint8_t[]> data);

    [[nodiscard]] std::size_t getLength() const;
    [[nodiscard]] std::shared_ptr<uint8_t[]> getData() const;
};

template <>
struct Deserialize<DataChunk> {
    static DataChunk fromStream(std::streampos position, std::fstream& fileStream);
};

template <>
struct Serialize<DataChunk> {
    static std::streampos toStream(const DataChunk &dataChunk, std::streampos position, std::fstream &fileStream);
};
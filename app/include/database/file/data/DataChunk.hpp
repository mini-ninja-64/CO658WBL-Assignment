#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <span>
#include <vector>

#include "database/file/parsing/common.hpp"

class DataChunk {
private:
  std::vector<uint8_t> data;

public:
  explicit DataChunk(const std::vector<uint8_t> &data);

  [[nodiscard]] const std::vector<uint8_t> &getData() const;
};

template <> struct Deserialize<DataChunk> {
  static DataChunk fromStream(std::streampos position,
                              std::fstream &fileStream);
};

template <> struct Serialize<DataChunk> {
  static std::streampos toStream(const DataChunk &dataChunk,
                                 std::streampos position,
                                 std::fstream &fileStream);
};

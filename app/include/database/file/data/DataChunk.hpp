#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <span>
#include <vector>

#include "LazyDataChunk.hpp"
#include "database/file/parsing/baseParsers.hpp"
#include "database/file/parsing/common.hpp"

/*
 * DataChunk Format:
 * | data               | size in bytes                         |
 * |--------------------|---------------------------------------|
 * | data length        | 4                                     |
 * | data chunk pointer | address length                        |
 * | bytes              | max chunk size - (4 + address length) |
 *
 */
template <typename ADDRESS> class DataChunk {
private:
  std::vector<uint8_t> data;
  std::optional<ADDRESS> nextChunk = std::nullopt;

public:
  explicit DataChunk(const std::vector<uint8_t> &data) : data(data){};
  DataChunk(const std::vector<uint8_t> &data,
            const std::optional<ADDRESS> &nextChunk)
      : data(data), nextChunk(nextChunk) {}

  [[nodiscard]] const std::vector<uint8_t> &getData() const { return data; }
  [[nodiscard]] const std::optional<ADDRESS> &getNextChunk() const {
    return nextChunk;
  };
};

struct DataChunkContext {
  size_t dataChunkSize;
};

template <typename ADDRESS>
struct Deserialize<DataChunk<ADDRESS>, DataChunkContext> {
  static DataChunk<ADDRESS> fromStream(std::streampos position,
                                       std::fstream &fileStream,
                                       DataChunkContext context) {

    auto buffer = new uint8_t[context.dataChunkSize]{0};
    fileStream.seekg(position);
    fileStream.read(reinterpret_cast<char *>(buffer),
                    std::streamsize(context.dataChunkSize));

    const auto dataBufferLength = UINT8_TO_UINT32(buffer, 0);

    const ADDRESS nextDataChunkAddress = Deserialize<ADDRESS>::fromBytes(
        std::span<uint8_t, Deserialize<ADDRESS>::length>{
            buffer + 4, Deserialize<ADDRESS>::length});
    std::optional<ADDRESS> nextDataChunk =
        nextDataChunkAddress == 0 ? std::nullopt
                                  : std::optional(nextDataChunkAddress);

    const auto dataStartPosition = 4 + Deserialize<ADDRESS>::length;
    std::vector<uint8_t> dataBuffer(dataBufferLength);
    std::memcpy(reinterpret_cast<char *>(dataBuffer.data()),
                buffer + dataStartPosition, dataBufferLength);

    delete[] buffer;
    return DataChunk<ADDRESS>(dataBuffer, nextDataChunk);
  }
};

template <typename ADDRESS>
struct Serialize<DataChunk<ADDRESS>, DataChunkContext> {
  static std::streampos toStream(const DataChunk<ADDRESS> &dataChunk,
                                 std::streampos position,
                                 std::fstream &fileStream,
                                 DataChunkContext context) {
    auto buffer = new uint8_t[context.dataChunkSize]{0};
    UINT32_TO_UINT8_ARRAY(buffer, 0, dataChunk.getData().size());

    if (dataChunk.getNextChunk()) {
      auto nextChunkAddress =
          Serialize<ADDRESS>::toBytes(dataChunk.getNextChunk().value());
      std::memcpy(buffer, nextChunkAddress.data(), Serialize<ADDRESS>::length);
    }

    const auto dataStartPosition = 4 + Deserialize<ADDRESS>::length;

    std::memcpy(buffer + dataStartPosition, dataChunk.getData().data(),
                dataChunk.getData().size());

    fileStream.seekp(position);
    fileStream.write(reinterpret_cast<char *>(buffer),
                     std::streamsize(context.dataChunkSize));
    delete[] buffer;
    return position + std::streamoff(context.dataChunkSize);
  }
};

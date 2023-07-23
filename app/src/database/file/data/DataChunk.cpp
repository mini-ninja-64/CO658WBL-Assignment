#include "database/file/data/DataChunk.hpp"

#include "utils/bitwise.hpp"

#include <iostream>

const std::vector<uint8_t> &DataChunk::getData() const { return data; }

DataChunk::DataChunk(const std::vector<uint8_t> &data) : data(data) {}

DataChunk Deserialize<DataChunk>::fromStream(std::streampos position,
                                             std::fstream &fileStream) {
  fileStream.seekg(position);
  std::array<uint8_t, 4> rawDataChunkLength{0};
  fileStream.read(reinterpret_cast<char *>(rawDataChunkLength.data()),
                  rawDataChunkLength.size());
  // TODO: check read lengths
  const auto dataChunkLength = UINT8_TO_UINT32(rawDataChunkLength, 0);

  std::vector<uint8_t> dataBuffer;
  dataBuffer.reserve(dataChunkLength);
  fileStream.read(reinterpret_cast<char *>(dataBuffer.data()), dataChunkLength);
  // TODO: check read lengths

  return DataChunk(dataBuffer);
}

std::streampos Serialize<DataChunk>::toStream(const DataChunk &dataChunk,
                                              std::streampos position,
                                              std::fstream &fileStream) {
  fileStream.seekp(position);

  auto bufferPosition = position;
  std::array<uint8_t, 4> rawDataChunkLength = {
      UINT32_TO_UINT8(dataChunk.getData().size())};
  fileStream.write(reinterpret_cast<char *>(rawDataChunkLength.data()),
                   rawDataChunkLength.size());
  bufferPosition += rawDataChunkLength.size();

  fileStream.write(reinterpret_cast<const char *>(dataChunk.getData().data()),
                   dataChunk.getData().size());
  bufferPosition += dataChunk.getData().size();

  return bufferPosition;
}

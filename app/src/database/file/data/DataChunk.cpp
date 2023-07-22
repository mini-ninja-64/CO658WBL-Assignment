#include "database/file/data/DataChunk.hpp"

#include "utils/bitwise.hpp"

DataChunk::DataChunk(size_t length, std::unique_ptr<uint8_t[]> data) :
        length(length),
        data(std::move(data)) {}

DataChunk::DataChunk(size_t length, std::shared_ptr<uint8_t[]> data) :
        length(length),
        data(std::move(data)) {}

std::size_t DataChunk::getLength() const {
    return length;
}

std::shared_ptr<uint8_t[]> DataChunk::getData() const {
    return data;
}

DataChunk Deserialize<DataChunk>::fromStream(std::streampos position, std::fstream& fileStream) {
    fileStream.seekg(position);
    std::array<uint8_t, 4> rawDataChunkLength{0};
    fileStream.read(reinterpret_cast<char *>(rawDataChunkLength.data()), rawDataChunkLength.size());
    // TODO: check read lengths
    const auto dataChunkLength = UINT8_TO_UINT32(rawDataChunkLength, 0);

    auto rawDataPointer = std::unique_ptr<uint8_t[]>(new uint8_t[dataChunkLength]);
    fileStream.read(reinterpret_cast<char *>(rawDataPointer.get()), dataChunkLength);
    // TODO: check read lengths

    return {
        dataChunkLength,
        std::move(rawDataPointer)
    };
}

std::streampos Serialize<DataChunk>::toStream(const DataChunk& dataChunk, std::streampos position, std::fstream& fileStream) {
    fileStream.seekp(position);

    auto bufferPosition = position;
    std::array<uint8_t, 4> rawDataChunkLength = { UINT32_TO_UINT8(dataChunk.getLength()) };
    fileStream.write(reinterpret_cast<char *>(rawDataChunkLength.data()), rawDataChunkLength.size());
    bufferPosition += rawDataChunkLength.size();

    fileStream.write(reinterpret_cast<char *>(dataChunk.getData().get()), dataChunk.getLength());
    bufferPosition += dataChunk.getLength();

    return bufferPosition;
}

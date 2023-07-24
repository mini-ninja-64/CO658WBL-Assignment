#pragma once

#include "DataChunk.hpp"
#include "DataMetadata.hpp"
#include "LazyDataChunk.hpp"
#include "database/file/Header.hpp"
#include "database/file/PullThroughCache.hpp"
#include "database/file/parsing/common.hpp"

template <typename ADDRESS> class DataChunk;
template <typename ADDRESS> class LazyDataChunk;

template <typename ADDRESS> class DataFile {
public:
  using DataChunkPointer = std::shared_ptr<DataChunk<ADDRESS>>;

private:
  std::fstream file;
  DataMetadata metadata;
  PullThroughCache<ADDRESS, DataChunkPointer, 100> cache;
  std::streampos insertionPosition;

  void writeMetadata() {
    constexpr auto metadataPosition = Serialize<DatabaseFileHeader>::length;
    Serialize<DataMetadata>::toStream(metadata, metadataPosition, file);
  }

  DataChunkPointer pullDataChunkFromFile(ADDRESS address) {
    auto node = Deserialize<DataChunk<ADDRESS>, DataChunkContext>::fromStream(
        address, file, {.dataChunkSize = metadata.dataChunkSize});
    return std::move(std::make_shared<DataChunk<ADDRESS>>(node));
  }

public:
  static const uint32_t MAGIC_NUMBER = 0x64617461;

  // TODO: Should assert defaultDataChunkSize is > minimum required
  DataFile(const std::filesystem::path &filePath, bool forceOverwrite,
           uint32_t defaultDataChunkSize)
      : metadata(
            {.numberOfDataChunks = 0, .dataChunkSize = defaultDataChunkSize}),
        cache([this](auto address) -> auto{
          return this->pullDataChunkFromFile(address);
        }) {
    const bool write = !exists(filePath) || forceOverwrite;

    auto streamConfig = std::ios::in | std::ios::out | std::ios::binary;
    if (forceOverwrite)
      streamConfig |= std::ios::trunc;

    file = std::fstream(filePath, streamConfig);

    if (write) {
      DatabaseFileHeader header = {MAGIC_NUMBER, 1};
      auto pointer = Serialize<DatabaseFileHeader>::toStream(header, 0, file);
      Serialize<DataMetadata>::toStream(metadata, pointer, file);
    } else {
      auto indexHeader = Deserialize<DatabaseFileHeader>::fromStream(0, file);
      if (indexHeader.magicNumber != MAGIC_NUMBER)
        throw std::domain_error(
            "provided index file has incorrect magic number");

      // TODO: std::streampos smaller than size_t (complier/system/arch
      // dependent)
      auto metadataPosition = Deserialize<DatabaseFileHeader>::length;
      metadata = Deserialize<DataMetadata>::fromStream(metadataPosition, file);
    }
    file.seekg(0, std::ios_base::end);
    insertionPosition = file.tellg();
  }

  std::shared_ptr<DataChunk<ADDRESS>> getData(ADDRESS address) {
    return cache.fetch(address);
  }

  LazyDataChunk<ADDRESS>
  insertData(std::shared_ptr<DataChunk<ADDRESS>> dataChunk) {
    const auto dataChunkAddress = static_cast<ADDRESS>(insertionPosition);
    insertionPosition =
        Serialize<DataChunk<ADDRESS>, DataChunkContext>::toStream(
            *dataChunk, dataChunkAddress, file,
            {.dataChunkSize = metadata.dataChunkSize});
    cache.populate(dataChunkAddress, dataChunk);

    metadata.numberOfDataChunks++;
    writeMetadata();
    return {this, dataChunkAddress};
  }

  void saveData(ADDRESS address, std::shared_ptr<DataChunk<ADDRESS>> dataChunk) {
    Serialize<DataChunk<ADDRESS>, DataChunkContext>::toStream(
        *dataChunk,
        address,
        file,
        {.dataChunkSize = metadata.dataChunkSize}
    );
    cache.populate(address, dataChunk);
  }

  const std::streampos &getInsertionPosition() const {
    return insertionPosition;
  }

  const DataMetadata &getMetadata() const { return metadata; }
};

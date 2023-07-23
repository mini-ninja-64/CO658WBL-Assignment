#pragma once

#include <cstddef>

#include "DataChunk.hpp"
#include "DataFile.hpp"

template <typename ADDRESS> class DataFile;

template <typename ADDRESS> class LazyDataChunk {
private:
  DataFile<ADDRESS> *dataFile;
  const ADDRESS address;

public:
  LazyDataChunk(DataFile<ADDRESS> *dataFile, const ADDRESS address)
      : dataFile(dataFile), address(address) {}

  ADDRESS getAddress() const { return address; }

  std::shared_ptr<DataChunk> get() { return dataFile->getData(address); }
};
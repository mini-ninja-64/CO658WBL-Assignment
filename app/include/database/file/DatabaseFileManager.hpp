#pragma once

#include <memory>
#include <fstream>
#include <filesystem>

#include "database/file/Header.hpp"
#include "database/file/index/IndexFile.hpp"
#include "database/file/data/DataFile.hpp"

template<typename K, typename ADDRESS>
class DatabaseFileManager {
private:
    IndexFile<K, ADDRESS> indexFile;
    DataFile<K, ADDRESS> dataFile;
public:
    DatabaseFileManager(const std::filesystem::path& indexFilePath,
                                 const std::filesystem::path& dataFilePath,
                                 bool forceOverwrite):
                                 indexFile(indexFilePath, forceOverwrite, 100),
                                 dataFile(dataFilePath, forceOverwrite) {
        indexFile.getRootNode().get();
    }

    // insertData(addr, const Data&)

};

#pragma once

#include <memory>
#include <fstream>
#include <filesystem>

#include "database/file/index/IndexFile.hpp"

template<typename K, typename ADDRESS>
class IndexFile;
template<typename K, typename ADDRESS>
class FileBackedNode;

template<typename K, typename ADDRESS>
class LazyNode {
private:
    IndexFile<K, ADDRESS>* indexFile;
    ADDRESS address;

public:
    LazyNode(IndexFile<K, ADDRESS>* indexFile, ADDRESS address):
        indexFile(indexFile),
        address(address) {}

    ADDRESS getAddress() const {
        return address;
    }

    /*
     * Returns the node
     * If available the node will be read from memory,
     * otherwise a file read will be carried out
     */
    std::shared_ptr<FileBackedNode<K, ADDRESS>> get() const {
        return indexFile->getNode(address);
    }

    bool operator==(const LazyNode &rhs) const {
        return address == rhs.address && indexFile == rhs.indexFile;
    }
};

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
    IndexFile<K, ADDRESS>& indexFile;
    const ADDRESS address;
    std::shared_ptr<FileBackedNode<K, ADDRESS>> node = nullptr;

public:
    LazyNode(IndexFile<K, ADDRESS>& indexFile, const ADDRESS address):
        indexFile(indexFile),
        address(address) {}

    LazyNode(IndexFile<K, ADDRESS> &indexFile, const ADDRESS address,
             const std::shared_ptr<FileBackedNode<K, ADDRESS>> &node):
                indexFile(indexFile),
                address(address),
                node(node) {}

    ADDRESS getAddress() const {
        return address;
    }

    /*
     * Returns the node
     * If available the node will be read from memory,
     * otherwise a file read will be carried out
     */
    std::shared_ptr<FileBackedNode<K, ADDRESS>> get() {
        if(node == nullptr) node = indexFile.getNode(address);
        return node;
    }

    /*
     * Unloads the node if stored in memory
     * If the node is currently loaded in memory, it will be unloaded and de-allocated.
     *
     * This is helpful when needing to free up memory in the system after carrying out an
     * operation where the node will not be required in the future so can be discarded
     */
    void unload() {
        node = nullptr;
    }
};

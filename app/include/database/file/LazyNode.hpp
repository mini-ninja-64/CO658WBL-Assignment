#pragma once

#include <memory>
#include <fstream>
#include <filesystem>
#include "DatabaseFileManager.hpp"

template<typename K, typename ADDRESS>
class FileBackedNode {
};

template<typename K, typename ADDRESS>
class LazyNode {
private:
//    const DatabaseFileManager<K, ADDRESS>& databaseFileManager;
    const ADDRESS address;
    std::shared_ptr<FileBackedNode<K, ADDRESS>> node = nullptr;

public:
    explicit LazyNode(const ADDRESS address) : address(address) {}

    std::shared_ptr<FileBackedNode<K, ADDRESS>> get() {
        if(node.get() == nullptr) node = readNode(address);
        return node;
    }
};

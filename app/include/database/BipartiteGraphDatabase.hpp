#pragma once
//
//#include <memory>
//#include <filesystem>
//#include <fstream>
//#include <array>
//#include "Ingredient.hpp"
//#include "database/b_plus_tree/BPlusTree.hpp"
//#include "utils/bitwise.hpp"
//#include "database/file/DatabaseFile.hpp"

//class BipartiteGraphDatabase {
//private:
//    constexpr const static GraphDatabaseHeader DEFAULT_HEADER = {
//            .magicNumber = 0xAABBCCDD,
//            .formatVersion = 1,
//            .BPlusSection = GRAPH_DATABASE_HEADER_SIZE,
//            .dataSection = GRAPH_DATABASE_HEADER_SIZE + 4 + 4
//    };
//
//    bool newFile;
//    std::fstream graphDatabaseFile;
//    GraphDatabaseHeader graphDatabaseHeader;
//    BPlusTree<500, uint32_t, uint32_t> bPlusTree;
//
//    void saveHeader() {
//        graphDatabaseFile.seekg(0);
//        const auto headerBuffer = serializeHeader(graphDatabaseHeader);
//        graphDatabaseFile.write(reinterpret_cast<const char *>(headerBuffer.data()), GRAPH_DATABASE_HEADER_SIZE);
//    }
//    void saveBPlusTree() {
//        graphDatabaseFile.seekg(graphDatabaseHeader.BPlusSection);
//
//    }
//
//public:
//    explicit BipartiteGraphDatabase(const std::filesystem::path& databasePath, bool forceNewFile):
//            newFile(forceNewFile || !exists(databasePath)),
//            graphDatabaseFile(databasePath, std::ios::in | std::ios::out | std::ios::binary),
//            graphDatabaseHeader(newFile ? DEFAULT_HEADER : deserializeHeader(graphDatabaseFile)) {
//        if (newFile) {
//            saveHeader();
//            saveBPlusTree();
//        }
//    }
//    explicit BipartiteGraphDatabase(const std::filesystem::path& databasePath):
//        BipartiteGraphDatabase(databasePath, false) {}
//
//};
//

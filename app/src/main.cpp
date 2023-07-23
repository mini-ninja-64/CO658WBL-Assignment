#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <chrono>
#include <random>
#include "database/BipartiteGraphDatabase.hpp"

#include "database/b_plus_tree/BPlusTree.hpp"
#include "database/file/FileBackedBPlusTree.hpp"


#ifndef TESTING

#define KEY_TYPE uint32_t
#define ADDRESS_TYPE uint32_t

int main() {
    FileBackedBPlusTree<KEY_TYPE, ADDRESS_TYPE> fileBackedBPlusTree("./test.graph_index", "./test.graph_db", true);
    DataChunk dataChunk = DataChunk({0,1,2,3});
    fileBackedBPlusTree.insert(100, dataChunk);
}

#endif
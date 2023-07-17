#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <chrono>
#include <random>
#include "database/BipartiteGraphDatabase.hpp"

#define RECIPES_PER_INGREDIENT 99999

//#include "BPlusTree.hpp"
#include "database/b_plus_tree/BPlusTree.hpp"
#include "database/file/DatabaseFileManager.hpp"


#ifndef TESTING

//#define ENTRIES_COUNT 10000000
//
//#include "database/b_plus_tree/Node.hpp"
//#include "database/b_plus_tree/Internal.hpp"
//#include "database/b_plus_tree/Leaf.hpp"

//auto now = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
//std::default_random_engine randomEngine(now);
//std::uniform_int_distribution<uint32_t> randomEntry(0, ENTRIES_COUNT);
//std::uniform_int_distribution<uint32_t> randomValue(0, 0xFFFFFFFF);

//#include "database/file/Header.hpp"

int main() {
    DatabaseFileManager<uint32_t, uint32_t> databaseFileManager("./test.graph_index", "./test.graph_db", true);

//    BipartiteGraphDatabase database("./test.graph_db");
//    std::cout << "Opening db version: ";

//#define ORDER 500
//    BPlusTree<uint32_t,  uint32_t> testTree(ORDER);
//    std::cout << "priming data" << std::endl;
//    for (int i = 0; i < ENTRIES_COUNT; ++i) {
//        auto itemValue = randomValue(randomEngine);
//        testTree.insert(i, itemValue);
//        if (i == 1 * (ENTRIES_COUNT/4)) std::cout << "25%" << std::endl;
//        if (i == 2 * (ENTRIES_COUNT/4)) std::cout << "50%" << std::endl;
//        if (i == 3 * (ENTRIES_COUNT/4)) std::cout << "75%" << std::endl;
//    }
//    std::cout << "data primed" << std::endl;
//
//    auto itemKey = randomEntry(randomEngine);
//    auto t1 = std::chrono::high_resolution_clock::now();
//    auto itemValue = testTree.find(itemKey);
//    auto t2 = std::chrono::high_resolution_clock::now();
//
//    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(t2-t1).count();
//    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(t2-t1).count();
//    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
//    std::cout << "estimated node count: " << ENTRIES_COUNT / ((ORDER - 2) / 2) << std::endl;
//    std::cout << "It took "
//        << nanoseconds << "ns, " << microseconds << "us, " << milliseconds << "ms to find '"
//        << itemKey
//        << "': '"
//        << itemValue.value()
//        << "' from "
//        << ENTRIES_COUNT << " entries, stored over "
//        << testTree.getNodeCount() << " nodes with an order of "
//        << ORDER
//        << std::endl;
}

#endif
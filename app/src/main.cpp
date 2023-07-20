#include <iostream>
#include <vector>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <chrono>
#include <random>
#include "database/BipartiteGraphDatabase.hpp"

#include "database/b_plus_tree/BPlusTree.hpp"
#include "database/file/DatabaseFileManager.hpp"


#ifndef TESTING

int main() {
    DatabaseFileManager<uint32_t, uint32_t> databaseFileManager("./test.graph_index", "./test.graph_db", true);
}

#endif
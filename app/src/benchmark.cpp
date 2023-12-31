#include "database/file/FileBackedBPlusTree.hpp"
#include "defaultConfig.hpp"
#include <iostream>
#include <random>

#if BENCHMARK_DB && !defined(TESTING)

#define KEY_TYPE uint32_t
#define ADDRESS_TYPE uint32_t

#define ENTRIES_COUNT 10000000

auto now = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now().time_since_epoch())
        .count();
std::default_random_engine randomEngine(now);
std::uniform_int_distribution<uint32_t> randomEntry(0, ENTRIES_COUNT);
std::uniform_int_distribution<uint32_t> randomValue(0, 0xFFFFFFFF);

int main() {
#define ORDER 500
    FileBackedBPlusTree<KEY_TYPE, ADDRESS_TYPE> testTree(
            "./test.graph_index", "./test.graph_db", ORDER, BLOCK_SIZE, true);

    std::cout << "priming data" << std::endl;
    for (int i = 0; i < ENTRIES_COUNT; ++i) {
        auto itemValue = randomValue(randomEngine);
        testTree.insert(i, {UINT32_TO_UINT8(itemValue)});
        if (i == 1 * (ENTRIES_COUNT / 4))
            std::cout << "25%" << std::endl;
        if (i == 2 * (ENTRIES_COUNT / 4))
            std::cout << "50%" << std::endl;
        if (i == 3 * (ENTRIES_COUNT / 4))
            std::cout << "75%" << std::endl;
        if (!testTree.find(i)) {
            std::cout << testTree.renderGraphVizView() << std::endl;
            std::cout << "broken" << std::endl;
        }
    }
    std::cout << "data primed" << std::endl;

    auto itemKey = randomEntry(randomEngine);
    auto t1 = std::chrono::high_resolution_clock::now();
    auto itemValue = testTree.find(itemKey);
    auto t2 = std::chrono::high_resolution_clock::now();

    auto nanoseconds =
            std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
    auto microseconds =
            std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
    auto milliseconds =
            std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

    std::cout << "estimated node count: " << ENTRIES_COUNT / ((ORDER - 2) / 2)
              << std::endl;
    std::cout << "It took " << nanoseconds << "ns, " << microseconds << "us, "
              << milliseconds << "ms to find '" << itemKey << "': '"
              << UINT8_TO_UINT32(itemValue.value(), 0) << "' from " << ENTRIES_COUNT
              << " entries, stored over "
              << testTree.getIndexFile().getMetadata().numberOfNodes
              << " nodes with an order of " << ORDER << std::endl;
}

#endif

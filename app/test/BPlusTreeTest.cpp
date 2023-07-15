#include "BPlusTree.hpp"

#include <gtest/gtest.h>

template<size_t ORDER, typename K>
BPlusTree<ORDER, K, K> createTreeWithKeyAsValue(std::initializer_list<K> insertions) {
    BPlusTree<ORDER, K, K> newTree;
    for (const auto &insertionKey: insertions) {
        newTree.insert(insertionKey, insertionKey);
    }
    return newTree;
}

TEST(BPlusTree, CorrectlyInsertsRecordsIntoTheRootNodeWithoutMakingChildren) {
    auto testTree = createTreeWithKeyAsValue<4, int>({0,1,2});

    ASSERT_EQ(testTree.getRoot().numberOfRecords(), 3);
}

TEST(BPlusTree, CorrectlyInsertsRecordsIntoTheRootNodeUntilItsFull) {
    auto testTree = createTreeWithKeyAsValue<5, int>({0,1,2,3,4});

    ASSERT_EQ(testTree.getRoot().numberOfRecords(), 1);
    ASSERT_EQ(testTree.getRoot().numberOfChildren(), 2);
    ASSERT_EQ(testTree.getRoot().getChildren()[0]->numberOfRecords(), 2);
    ASSERT_EQ(testTree.getRoot().getChildren()[1]->numberOfRecords(), 3);
}

TEST(BPlusTree, CorrectlyInsertsRecordsIntoTheCorrectChildNode) {
    auto testTree = createTreeWithKeyAsValue<4, int>({0,1,2,3,4});

    ASSERT_EQ(testTree.getRoot().numberOfRecords(), 1);
    ASSERT_EQ(testTree.getRoot().numberOfChildren(), 2);
    ASSERT_EQ(testTree.getRoot().getChildren()[0]->numberOfRecords(), 2);
    ASSERT_EQ(testTree.getRoot().getChildren()[1]->numberOfRecords(), 3);
}

TEST(BPlusTree, CorrectlyCreatesASmallSimpleTreeWithASingleKeyInTheRoot) {
    auto testTree = createTreeWithKeyAsValue<3, int>({0,1,2,3,4});

    ASSERT_EQ(testTree.getRoot().numberOfRecords(), 1);
    ASSERT_EQ(testTree.getRoot().getRecords()[0], 2);
    ASSERT_EQ(testTree.getRoot().numberOfChildren(), 2);

    ASSERT_EQ(testTree.getRoot().getChildren()[0]->numberOfRecords(), 1);
    ASSERT_EQ(testTree.getRoot().getChildren()[0]->numberOfChildren(), 2);

    ASSERT_EQ(testTree.getRoot().getChildren()[1]->numberOfRecords(), 1);
    ASSERT_EQ(testTree.getRoot().getChildren()[1]->numberOfChildren(), 2);
}

TEST(BPlusTree, CorrectlyCreatesASmallSimpleTreeWhenUnsortedKeysAreInserted) {
    auto testTree = createTreeWithKeyAsValue<3, int>({0,4,3,1,2});

    ASSERT_EQ(testTree.getRoot().numberOfRecords(), 2);
    ASSERT_EQ(testTree.getRoot().getRecords()[0], 1);
    ASSERT_EQ(testTree.getRoot().getRecords()[1], 3);
    ASSERT_EQ(testTree.getRoot().numberOfChildren(), 3);

    ASSERT_EQ(testTree.getRoot().getChildren()[0]->numberOfRecords(), 1);
    ASSERT_EQ(testTree.getRoot().getChildren()[1]->numberOfRecords(), 2);
    ASSERT_EQ(testTree.getRoot().getChildren()[2]->numberOfRecords(), 2);
}

TEST(BPlusTree, CorrectlyCreatesALargeSimpleTreeWithMultipleKeysInTheRoot) {
    auto testTree = createTreeWithKeyAsValue<4, int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});

    ASSERT_EQ(testTree.getRoot().numberOfRecords(), 2);
    ASSERT_EQ(testTree.getRoot().getRecords()[0], 6);
    ASSERT_EQ(testTree.getRoot().getRecords()[1], 12);
    ASSERT_EQ(testTree.getRoot().numberOfChildren(), 3);
    ASSERT_EQ(testTree.getRoot().getChildren()[0]->numberOfRecords(), 2);
    ASSERT_EQ(testTree.getRoot().getChildren()[0]->numberOfChildren(), 3);

    ASSERT_EQ(testTree.getRoot().getChildren()[1]->numberOfRecords(), 2);
    ASSERT_EQ(testTree.getRoot().getChildren()[1]->numberOfChildren(), 3);

    ASSERT_EQ(testTree.getRoot().getChildren()[2]->numberOfRecords(), 1);
    ASSERT_EQ(testTree.getRoot().getChildren()[2]->numberOfChildren(), 2);
}

TEST(BPlusTree, CorrectlyCreatesALargeSimpleTreeWithMultipleKeysInTheRootUsingUnorderedInsertions) {
    auto testTree = createTreeWithKeyAsValue<4, int>({3,10,0,5,4,11,1,6,2,14,7,8,15,9,13,12});

    ASSERT_EQ(testTree.getRoot().numberOfRecords(), 1);
    ASSERT_EQ(testTree.getRoot().getRecords()[0], 7);
    ASSERT_EQ(testTree.getRoot().numberOfChildren(), 2);

    ASSERT_EQ(testTree.getRoot().getChildren()[0]->numberOfRecords(), 2);
    ASSERT_EQ(testTree.getRoot().getChildren()[0]->numberOfChildren(), 3);

    ASSERT_EQ(testTree.getRoot().getChildren()[1]->numberOfRecords(), 3);
    ASSERT_EQ(testTree.getRoot().getChildren()[1]->numberOfChildren(), 4);
}

TEST(BPlusTree, CorrectlyFindsItemsInATree) {
    BPlusTree<5, int, int> testTree;
    testTree.insert(1, 50);
    testTree.insert(8, 400);
    testTree.insert(2, 100);
    testTree.insert(0, 0);
    testTree.insert(6, 300);
    testTree.insert(5, 250);
    testTree.insert(3, 150);
    testTree.insert(4, 200);
    testTree.insert(7, 350);

    ASSERT_EQ(testTree.find(0)->get(), 0);
    ASSERT_EQ(testTree.find(1)->get(), 50);
    ASSERT_EQ(testTree.find(2)->get(), 100);
    ASSERT_EQ(testTree.find(4)->get(), 200);
    ASSERT_EQ(testTree.find(7)->get(), 350);
    ASSERT_FALSE(testTree.find(9));
}

TEST(BPlusTree, CorrectlyDeletesElementOnlyPresentInLeafNode) {
    auto testTree = createTreeWithKeyAsValue<4, int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});

    std::cout << testTree.getRoot().renderGraphVizView();
}

TEST(Node, CorrectlyFindsAMatchingLeaf) {
    Node<int, int> testLeaf1(nullptr, {0}, {}, {}, true);
    Node<int, int> testLeaf2(nullptr, {1}, {}, {}, true);
    Node<int, int> testLeaf3(nullptr, {2}, {}, {}, true);
    Node<int, int> testLeaf4(nullptr, {3, 4}, {}, {}, true);

    Node<int, int> testInternal1(nullptr, {1}, {}, {}, false);
    testInternal1.addChild(0, &testLeaf1);
    testInternal1.addChild(1, &testLeaf2);

    Node<int, int> testInternal2(nullptr, {3}, {}, {}, false);
    testInternal2.addChild(0, &testLeaf3);
    testInternal2.addChild(1, &testLeaf4);

    Node<int, int> testRootNode(nullptr, {2}, {}, {}, false);
    testRootNode.addChild(0, &testInternal1);
    testRootNode.addChild(1, &testInternal2);

    ASSERT_EQ(testRootNode.findLeaf(2), &testLeaf3);
    ASSERT_EQ(testRootNode.findLeaf(3)->numberOfRecords(), 2);
}

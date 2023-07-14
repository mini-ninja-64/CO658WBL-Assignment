#include "BPlusTree.hpp"

#include <gtest/gtest.h>

TEST(BPlusTree, CorrectlyInsertsRecordsIntoTheRootNodeWithoutMakingChildren) {
    BPlusTree<4, int, int> testTree;

    testTree.insert(0, 1);
    testTree.insert(1, 1);
    testTree.insert(2, 1);

    ASSERT_EQ(testTree.getRoot().numberOfRecords(), 3);
}

TEST(BPlusTree, CorrectlyInsertsRecordsIntoTheRootNodeUntilItsFull) {
    BPlusTree<5, int, int> testTree;

    testTree.insert(0, 1);
    testTree.insert(1, 1);
    testTree.insert(2, 1);
    testTree.insert(3, 1);
    testTree.insert(4, 1);

    ASSERT_EQ(testTree.getRoot().numberOfRecords(), 1);
    ASSERT_EQ(testTree.getRoot().numberOfChildren(), 2);
    ASSERT_EQ(testTree.getRoot().getChildren()[0]->numberOfRecords(), 2);
    ASSERT_EQ(testTree.getRoot().getChildren()[1]->numberOfRecords(), 3);
}

TEST(BPlusTree, CorrectlyInsertsRecordsIntoTheCorrectChildNode) {
    BPlusTree<4, int, int> testTree;

    testTree.insert(0, 1);
    testTree.insert(1, 1);
    testTree.insert(2, 1);
    testTree.insert(3, 1);
    testTree.insert(4, 1);


    std::cout << testTree.renderGraphViz() << std::endl;
    ASSERT_EQ(testTree.getRoot().numberOfRecords(), 1);
    ASSERT_EQ(testTree.getRoot().numberOfChildren(), 2);
    ASSERT_EQ(testTree.getRoot().getChildren()[0]->numberOfRecords(), 2);
    ASSERT_EQ(testTree.getRoot().getChildren()[1]->numberOfRecords(), 3);
}

TEST(BPlusTree, CorrectlyCreatesASmallSimpleTreeWithASingleKeyInTheRoot) {
    BPlusTree<3, int, int> testTree;

    testTree.insert(0, 1);
    testTree.insert(1, 1);
    testTree.insert(2, 1);
    testTree.insert(3, 1);
    testTree.insert(4, 1);

    std::cout << testTree.renderGraphViz() << std::endl;
    ASSERT_EQ(testTree.getRoot().numberOfRecords(), 1);
    ASSERT_EQ(testTree.getRoot().getRecords()[0], 2);
    ASSERT_EQ(testTree.getRoot().numberOfChildren(), 2);

    ASSERT_EQ(testTree.getRoot().getChildren()[0]->numberOfRecords(), 1);
    ASSERT_EQ(testTree.getRoot().getChildren()[0]->numberOfChildren(), 2);

    ASSERT_EQ(testTree.getRoot().getChildren()[1]->numberOfRecords(), 1);
    ASSERT_EQ(testTree.getRoot().getChildren()[1]->numberOfChildren(), 2);
}

TEST(BPlusTree, CorrectlyCreatesASmallSimpleTreeWhenUnsortedKeysAreInserted) {
    BPlusTree<3, int, int> testTree;

    testTree.insert(0, 1);
    testTree.insert(4, 1);
    testTree.insert(3, 1);
    testTree.insert(1, 1);
    testTree.insert(2, 1);

    std::cout << testTree.renderGraphViz() << std::endl;
    ASSERT_EQ(testTree.getRoot().numberOfRecords(), 2);
    ASSERT_EQ(testTree.getRoot().getRecords()[0], 1);
    ASSERT_EQ(testTree.getRoot().getRecords()[1], 3);
    ASSERT_EQ(testTree.getRoot().numberOfChildren(), 3);

    ASSERT_EQ(testTree.getRoot().getChildren()[0]->numberOfRecords(), 1);
    ASSERT_EQ(testTree.getRoot().getChildren()[1]->numberOfRecords(), 2);
    ASSERT_EQ(testTree.getRoot().getChildren()[2]->numberOfRecords(), 2);
}

TEST(BPlusTree, CorrectlyCreatesALargeSimpleTreeWithMultipleKeysInTheRoot) {
    BPlusTree<4, int, int> testTree;

    testTree.insert(0, 1);
    testTree.insert(1, 1);
    testTree.insert(2, 1);
    testTree.insert(3, 1);
    testTree.insert(4, 1);
    testTree.insert(5, 1);
    testTree.insert(6, 1);
    testTree.insert(7, 1);
    testTree.insert(8, 1);
    testTree.insert(9, 1);
    testTree.insert(10, 1);
    testTree.insert(11, 1);
    testTree.insert(12, 1);
    testTree.insert(13, 1);
    testTree.insert(14, 1);
    testTree.insert(15, 1);

    std::cout << testTree.renderGraphViz() << std::endl;
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
    BPlusTree<4, int, int> testTree;

    testTree.insert(3, 1);
    testTree.insert(10, 1);
    testTree.insert(0, 1);
    testTree.insert(5, 1);
    testTree.insert(4, 1);
    testTree.insert(11, 1);
    testTree.insert(1, 1);
    testTree.insert(6, 1);
    testTree.insert(2, 1);
    testTree.insert(14, 1);
    testTree.insert(7, 1);
    testTree.insert(8, 1);
    testTree.insert(15, 1);
    testTree.insert(9, 1);
    testTree.insert(13, 1);
    testTree.insert(12, 1);

    std::cout << testTree.renderGraphViz() << std::endl;
    ASSERT_EQ(testTree.getRoot().numberOfRecords(), 1);
    ASSERT_EQ(testTree.getRoot().getRecords()[0], 7);
    ASSERT_EQ(testTree.getRoot().numberOfChildren(), 2);

    ASSERT_EQ(testTree.getRoot().getChildren()[0]->numberOfRecords(), 2);
    ASSERT_EQ(testTree.getRoot().getChildren()[0]->numberOfChildren(), 3);

    ASSERT_EQ(testTree.getRoot().getChildren()[1]->numberOfRecords(), 3);
    ASSERT_EQ(testTree.getRoot().getChildren()[1]->numberOfChildren(), 4);
}

TEST(Node, CorrectlyFindsAMatchingLeaf) {
    Node<int, int> testLeaf1(nullptr, {0}, {}, {}, true);
    Node<int, int> testLeaf2(nullptr, {1}, {}, {}, true);
    Node<int, int> testLeaf3(nullptr, {2}, {}, {}, true);
    Node<int, int> testLeaf4(nullptr, {3,4}, {}, {}, true);

    Node<int, int> testInternal1(nullptr, {1}, {}, {}, false);
    testInternal1.addChild(&testLeaf1);
    testInternal1.addChild(&testLeaf2);
    Node<int, int> testInternal2(nullptr, {3}, {}, {}, false);
    testInternal2.addChild(&testLeaf3);
    testInternal2.addChild(&testLeaf4);

    Node<int, int> testRootNode(nullptr, {2}, {}, {}, false);
    testRootNode.addChild(&testInternal1);
    testRootNode.addChild(&testInternal2);

    ASSERT_EQ(testRootNode.findLeaf(2), &testLeaf3);
    ASSERT_EQ(testRootNode.findLeaf(3)->numberOfRecords(), 2);
}


//TEST(Node, TestComplexGraph) {
//    Node<int, int> testLeaf1({0}, {}, {}, true);
//    Node<int, int> testLeaf2({1}, {}, {}, true);
//    Node<int, int> testLeaf3({2}, {}, {}, true);
//    Node<int, int> testLeaf4({3,4}, {}, {}, true);
//
//    Node<int, int> testInternal1({1}, {&testLeaf1, &testLeaf2}, {}, false);
//    Node<int, int> testInternal2({3}, {&testLeaf3, &testLeaf4}, {}, false);
//
//    Node<int, int> testRootNode({2}, {&testInternal1, &testInternal2}, {}, false);
//
//    BPlusTree<3, int, int> tree;
//    tree.getRoot().getRecords().push_back(2);
//    tree.getRoot().getChildren().push_back(&testInternal1);
//    tree.getRoot().getChildren().push_back(&testInternal2);
//
//    std::cout << tree.renderGraphViz();
//
//    ASSERT_EQ(testRootNode.findLeaf(2), &testLeaf3);
//    ASSERT_EQ(testRootNode.findLeaf(3)->numberOfRecords(), 2);
//}

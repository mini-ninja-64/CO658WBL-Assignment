//#include "database/b_plus_tree/BPlusTree.hpp"
//
//#include <gtest/gtest.h>
//
//template<size_t ORDER, typename K>
//BPlusTree<ORDER, K, K> createTreeWithKeyAsValue(std::initializer_list<K> insertions) {
//    BPlusTree<ORDER, K, K> newTree;
//    for (const auto &insertionKey: insertions) {
//        newTree.insert(insertionKey, insertionKey);
//    }
//    return newTree;
//}
//
//template<typename K, typename V>
//Internal<K, V>* uncheckedInternalNode(Node<K, V>* nodePointer) {
//    return dynamic_cast<Internal<K, V>*>(nodePointer);
//}
//
//template<typename K, typename V>
//Leaf<K, V>* uncheckedLeaf(Node<K, V>* nodePointer) {
//    return dynamic_cast<Leaf<K, V>*>(nodePointer);
//}
//
//TEST(BPlusTree, CorrectlyInsertsRecordsIntoTheRootNodeWithoutMakingChildren) {
//    auto testTree = createTreeWithKeyAsValue<4, int>({0,1,2});
//
//    ASSERT_EQ(testTree.getRoot()->getRecords().size(), 3);
//}
//
//TEST(BPlusTree, CorrectlyInsertsRecordsIntoTheRootNodeUntilItsFull) {
//    auto testTree = createTreeWithKeyAsValue<5, int>({0,1,2,3,4});
//
//    ASSERT_EQ(testTree.getRoot()->getRecords().size(), 1);
//    auto root = uncheckedInternalNode(testTree.getRoot());
//    ASSERT_EQ(root->getChildren().size(), 2);
//    ASSERT_EQ(root->getChildren()[0]->getRecords().size(), 2);
//    ASSERT_EQ(root->getChildren()[1]->getRecords().size(), 3);
//}
//
//TEST(BPlusTree, CorrectlyCreatesASmallSimpleTreeWithASingleKeyInTheRoot) {
//    auto testTree = createTreeWithKeyAsValue<3, int>({0,1,2,3,4});
//
//    std::cout << testTree.renderGraphVizView() << std::endl;
//    auto root = uncheckedInternalNode(testTree.getRoot());
//    ASSERT_EQ(root->getRecords().size(), 1);
//    ASSERT_EQ(root->getRecords()[0], 2);
//    ASSERT_EQ(root->getChildren().size(), 2);
//
//    ASSERT_EQ(root->getChildren()[0]->getRecords().size(), 1);
//    ASSERT_EQ(uncheckedInternalNode(root->getChildren()[0])->getChildren().size(), 2);
//
//    ASSERT_EQ(root->getChildren()[1]->getRecords().size(), 1);
//    ASSERT_EQ(uncheckedInternalNode(root->getChildren()[1])->getChildren().size(), 2);
//}
//
//TEST(BPlusTree, CorrectlyTraversesTreeUpwardsToRebalanceParents) {
//    auto testTree = createTreeWithKeyAsValue<3, int>({0,1,2,3,4,5,6,7,8,9});
//
//    std::cout << testTree.renderGraphVizView() << std::endl;
//    auto root = uncheckedInternalNode(testTree.getRoot());
//    ASSERT_EQ(root->getRecords().size(), 1);
//    ASSERT_EQ(root->getRecords()[0], 4);
//    ASSERT_EQ(root->getChildren().size(), 2);
//
//    ASSERT_EQ(root->getChildren()[0]->getRecords().size(), 1);
//    ASSERT_EQ(uncheckedInternalNode(root->getChildren()[0])->getChildren().size(), 2);
//
//    ASSERT_EQ(root->getChildren()[1]->getRecords().size(), 1);
//    ASSERT_EQ(uncheckedInternalNode(root->getChildren()[1])->getChildren().size(), 2);
//}
//
//TEST(BPlusTree, CorrectlyCreatesASmallSimpleLeftLeaningTreeWithASingleKeyInTheRoot) {
//    auto testTree = createTreeWithKeyAsValue<4, int>({0,1,2,3,4,5,6,7,8,9});
//
//    std::cout << testTree.renderGraphVizView() << std::endl;
//    auto root = uncheckedInternalNode(testTree.getRoot());
//    ASSERT_EQ(root->getRecords().size(), 1);
//    ASSERT_EQ(root->getRecords()[0], 6);
//    ASSERT_EQ(root->getChildren().size(), 2);
//
//    ASSERT_EQ(root->getChildren()[0]->getRecords().size(), 2);
//    ASSERT_EQ(uncheckedInternalNode(root->getChildren()[0])->getChildren().size(), 3);
//
//    ASSERT_EQ(root->getChildren()[1]->getRecords().size(), 1);
//    ASSERT_EQ(uncheckedInternalNode(root->getChildren()[1])->getChildren().size(), 2);
//}
//
//TEST(BPlusTree, CorrectlyCreatesASmallSimpleTreeWhenUnsortedKeysAreInserted) {
//    auto testTree = createTreeWithKeyAsValue<3, int>({0,4,3,1,2});
//
//    auto root = uncheckedInternalNode(testTree.getRoot());
//    ASSERT_EQ(root->getRecords().size(), 2);
//    ASSERT_EQ(root->getRecords()[0], 1);
//    ASSERT_EQ(root->getRecords()[1], 3);
//    ASSERT_EQ(root->getChildren().size(), 3);
//
//    ASSERT_EQ(root->getChildren()[0]->getRecords().size(), 1);
//    ASSERT_EQ(root->getChildren()[1]->getRecords().size(), 2);
//    ASSERT_EQ(root->getChildren()[2]->getRecords().size(), 2);
//}
//
//TEST(BPlusTree, CorrectlyCreatesALargeSimpleTreeWithMultipleKeysInTheRoot) {
//    auto testTree = createTreeWithKeyAsValue<4, int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});
//
//    auto root = uncheckedInternalNode(testTree.getRoot());
//    ASSERT_EQ(root->getRecords().size(), 2);
//    ASSERT_EQ(root->getRecords()[0], 6);
//    ASSERT_EQ(root->getRecords()[1], 12);
//    ASSERT_EQ(root->getChildren().size(), 3);
//
//    auto internalNode0 = uncheckedInternalNode(root->getChildren()[0]);
//    ASSERT_EQ(internalNode0->getRecords().size(), 2);
//    ASSERT_EQ(internalNode0->getChildren().size(), 3);
//
//    auto internalNode1 = uncheckedInternalNode(root->getChildren()[1]);
//    ASSERT_EQ(internalNode1->getRecords().size(), 2);
//    ASSERT_EQ(internalNode1->getChildren().size(), 3);
//
//    auto internalNode2 = uncheckedInternalNode(root->getChildren()[2]);
//    ASSERT_EQ(internalNode2->getRecords().size(), 1);
//    ASSERT_EQ(internalNode2->getChildren().size(), 2);
//}
//
//TEST(BPlusTree, CorrectlyCreatesALargeSimpleTreeFromUnorderedInsertions) {
//    auto testTree = createTreeWithKeyAsValue<4, int>({3,10,0,5,4,11,1,6,2,14,7,8,15,9,13,12});
//
//    auto root = uncheckedInternalNode(testTree.getRoot());
//    ASSERT_EQ(root->getRecords()[0], 7);
//    ASSERT_EQ(root->getRecords().size(), 1);
//    ASSERT_EQ(root->getChildren().size(), 2);
//
//    auto internalNode0 = uncheckedInternalNode(root->getChildren()[0]);
//    ASSERT_EQ(internalNode0->getRecords().size(), 2);
//    ASSERT_EQ(internalNode0->getChildren().size(), 3);
//
//    auto internalNode1 = uncheckedInternalNode(root->getChildren()[1]);
//    ASSERT_EQ(internalNode1->getRecords().size(), 3);
//    ASSERT_EQ(internalNode1->getChildren().size(), 4);
//}
//
//TEST(BPlusTree, CorrectlyFindsItemsInATree) {
//    BPlusTree<5, int, int> testTree;
//    testTree.insert(1, 50);
//    testTree.insert(8, 400);
//    testTree.insert(2, 100);
//    testTree.insert(0, 0);
//    testTree.insert(6, 300);
//    testTree.insert(5, 250);
//    testTree.insert(3, 150);
//    testTree.insert(4, 200);
//    testTree.insert(7, 350);
//
//    std::cout << testTree.renderGraphVizView() << std::endl;
//    ASSERT_EQ(testTree.find(0), 0);
//    ASSERT_EQ(testTree.find(1), 50);
//    ASSERT_EQ(testTree.find(2), 100);
//    ASSERT_EQ(testTree.find(4), 200);
//    ASSERT_EQ(testTree.find(7), 350);
//    ASSERT_FALSE(testTree.find(9).has_value());
//}
//
//TEST(BPlusTree, CorrectlyDeletesElementOnlyPresentInLeafNode) {
//    auto testTree = createTreeWithKeyAsValue<4, int>({0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});
//    ASSERT_TRUE(testTree.find(6).has_value());
//    testTree.remove(6);
//
//    // Test either side to ensure the surrounding records have not been affected
//    ASSERT_EQ(testTree.find(5), 5);
//    ASSERT_FALSE(testTree.find(6).has_value());
//    ASSERT_EQ(testTree.find(7), 7);
//}
//

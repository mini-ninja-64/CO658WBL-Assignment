
#include "database/file/FileBackedBPlusTree.hpp"
#include <gtest/gtest.h>

#define INDEX_FILE "/tmp/db.graph_index"
#define DATA_FILE "/tmp/db.graph_db"
#define ADDRESS_TYPE uint32_t

template <size_t ORDER, size_t DEFAULT_CHUNK_SIZE, typename K>
FileBackedBPlusTree<K, ADDRESS_TYPE>
createTreeWithKeyAsValue(std::initializer_list<uint8_t> insertions) {
  FileBackedBPlusTree<K, ADDRESS_TYPE> newTree(INDEX_FILE, DATA_FILE, ORDER,
                                               DEFAULT_CHUNK_SIZE, true);
  for (const auto &insertionKey : insertions) {
    newTree.insert(insertionKey, {insertionKey});
    std::cout << "\n\n";
    std::cout << newTree.renderGraphVizView() << std::endl;
  }
  return newTree;
}

template <typename K, typename ADDRESS>
std::shared_ptr<FileBackedInternal<K, ADDRESS>>
uncheckedInternalNode(std::shared_ptr<FileBackedNode<K, ADDRESS>> nodePointer) {
  return std::dynamic_pointer_cast<FileBackedInternal<K, ADDRESS>>(nodePointer);
}

TEST(BPlusTree, CorrectlyInsertsRecordsIntoTheRootNodeWithoutMakingChildren) {
  auto testTree = createTreeWithKeyAsValue<4, 4096, uint32_t>({0, 1, 2});

  ASSERT_EQ(testTree.getRoot().get()->getRecords().size(), 3);
}

TEST(BPlusTree, CorrectlyInsertsRecordsIntoTheRootNodeUntilItsFull) {
  auto testTree = createTreeWithKeyAsValue<5, 4096, uint32_t>({0, 1, 2, 3, 4});

  ASSERT_EQ(testTree.getRoot().get()->getRecords().size(), 1);
  auto root = uncheckedInternalNode(testTree.getRoot().get());
  ASSERT_EQ(root->getChildrenAddresses().size(), 2);
  ASSERT_EQ(root->getChild(0).get()->getRecords().size(), 2);
  ASSERT_EQ(root->getChild(1).get()->getRecords().size(), 3);
}

TEST(BPlusTree, CorrectlyCreatesASmallSimpleTreeWithASingleKeyInTheRoot) {
  auto testTree = createTreeWithKeyAsValue<3, 4096, uint32_t>({0, 1, 2, 3, 4});

  auto root = uncheckedInternalNode(testTree.getRoot().get());
  ASSERT_EQ(root->getRecords().size(), 1);
  ASSERT_EQ(root->getRecords()[0], 2);
  ASSERT_EQ(root->getChildrenAddresses().size(), 2);

  ASSERT_EQ(root->getChild(0).get()->getRecords().size(), 1);
  ASSERT_EQ(uncheckedInternalNode(root->getChild(0).get())
                .get()
                ->getChildrenAddresses()
                .size(),
            2);

  ASSERT_EQ(root->getChild(1).get()->getRecords().size(), 1);
  ASSERT_EQ(uncheckedInternalNode(root->getChild(1).get())
                .get()
                ->getChildrenAddresses()
                .size(),
            2);
}

TEST(BPlusTree, CorrectlyTraversesTreeUpwardsToRebalanceParents) {
  auto testTree = createTreeWithKeyAsValue<3, 4096, uint32_t>(
      {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});

  auto root = uncheckedInternalNode(testTree.getRoot().get());
  ASSERT_EQ(root->getRecords().size(), 1);
  ASSERT_EQ(root->getRecords()[0], 4);
  ASSERT_EQ(root->getChildrenAddresses().size(), 2);

  ASSERT_EQ(root->getChild(0).get()->getRecords().size(), 1);
  ASSERT_EQ(uncheckedInternalNode(root->getChild(0).get())
                .get()
                ->getChildrenAddresses()
                .size(),
            2);

  ASSERT_EQ(root->getChild(1).get()->getRecords().size(), 1);
  ASSERT_EQ(uncheckedInternalNode(root->getChild(1).get())
                .get()
                ->getChildrenAddresses()
                .size(),
            2);
}

TEST(BPlusTree,
     CorrectlyCreatesASmallSimpleLeftLeaningTreeWithASingleKeyInTheRoot) {
  auto testTree = createTreeWithKeyAsValue<4, 4096, uint32_t>(
      {0, 1, 2, 3, 4, 5, 6, 7, 8, 9});

  auto root = uncheckedInternalNode(testTree.getRoot().get());
  ASSERT_EQ(root->getRecords().size(), 1);
  ASSERT_EQ(root->getRecords()[0], 6);
  ASSERT_EQ(root->getChildrenAddresses().size(), 2);

  ASSERT_EQ(root->getChild(0).get()->getRecords().size(), 2);
  ASSERT_EQ(uncheckedInternalNode(root->getChild(0).get())
                .get()
                ->getChildrenAddresses()
                .size(),
            3);

  ASSERT_EQ(root->getChild(1).get()->getRecords().size(), 1);
  ASSERT_EQ(uncheckedInternalNode(root->getChild(1).get())
                .get()
                ->getChildrenAddresses()
                .size(),
            2);
}

TEST(BPlusTree, CorrectlyCreatesASmallSimpleTreeWhenUnsortedKeysAreInserted) {
  auto testTree = createTreeWithKeyAsValue<3, 4096, uint32_t>({0, 4, 3, 1, 2});

  auto root = uncheckedInternalNode(testTree.getRoot().get());
  ASSERT_EQ(root->getRecords().size(), 2);
  ASSERT_EQ(root->getRecords()[0], 1);
  ASSERT_EQ(root->getRecords()[1], 3);
  ASSERT_EQ(root->getChildrenAddresses().size(), 3);

  ASSERT_EQ(root->getChild(0).get()->getRecords().size(), 1);
  ASSERT_EQ(root->getChild(1).get()->getRecords().size(), 2);
  ASSERT_EQ(root->getChild(2).get()->getRecords().size(), 2);
}

TEST(BPlusTree, CorrectlyCreatesALargeSimpleTreeWithMultipleKeysInTheRoot) {
  auto testTree = createTreeWithKeyAsValue<4, 4096, uint32_t>(
      {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15});

  auto root = uncheckedInternalNode(testTree.getRoot().get());
  ASSERT_EQ(root->getRecords().size(), 2);
  ASSERT_EQ(root->getRecords()[0], 6);
  ASSERT_EQ(root->getRecords()[1], 12);
  ASSERT_EQ(root->getChildrenAddresses().size(), 3);

  auto internalNode0 = uncheckedInternalNode(root->getChild(0).get());
  ASSERT_EQ(internalNode0->getRecords().size(), 2);
  ASSERT_EQ(internalNode0.get()->getChildrenAddresses().size(), 3);

  auto internalNode1 = uncheckedInternalNode(root->getChild(1).get());
  ASSERT_EQ(internalNode1->getRecords().size(), 2);
  ASSERT_EQ(internalNode1.get()->getChildrenAddresses().size(), 3);

  auto internalNode2 = uncheckedInternalNode(root->getChild(2).get());
  ASSERT_EQ(internalNode2->getRecords().size(), 1);
  ASSERT_EQ(internalNode2.get()->getChildrenAddresses().size(), 2);
}

TEST(BPlusTree, CorrectlyCreatesALargeSimpleTreeFromUnorderedInsertions) {
  auto testTree = createTreeWithKeyAsValue<4, 4096, uint32_t>(
      {3, 10, 0, 5, 4, 11, 1, 6, 2, 14, 7, 8, 15, 9, 13, 12});

  auto root = uncheckedInternalNode(testTree.getRoot().get());
  ASSERT_EQ(root->getRecords()[0], 7);
  ASSERT_EQ(root->getRecords().size(), 1);
  ASSERT_EQ(root->getChildrenAddresses().size(), 2);

  auto internalNode0 = uncheckedInternalNode(root->getChild(0).get());
  ASSERT_EQ(internalNode0->getRecords().size(), 2);
  ASSERT_EQ(internalNode0.get()->getChildrenAddresses().size(), 3);

  auto internalNode1 = uncheckedInternalNode(root->getChild(1).get());
  ASSERT_EQ(internalNode1->getRecords().size(), 3);
  ASSERT_EQ(internalNode1.get()->getChildrenAddresses().size(), 4);
}

TEST(BPlusTree, CorrectlyFindsItemsInATree) {
  FileBackedBPlusTree<uint32_t, uint32_t> testTree(INDEX_FILE, DATA_FILE, 5,
                                                   4096, true);
  testTree.insert(1, {50});
  testTree.insert(8, {65});
  testTree.insert(2, {100});
  testTree.insert(0, {0});
  testTree.insert(6, {80});
  testTree.insert(5, {250});
  testTree.insert(3, {150});
  testTree.insert(4, {200});
  testTree.insert(7, {125});

  ASSERT_EQ(testTree.find(0).value()[0], 0);
  ASSERT_EQ(testTree.find(1).value()[0], 50);
  ASSERT_EQ(testTree.find(2).value()[0], 100);
  ASSERT_EQ(testTree.find(4).value()[0], 200);
  ASSERT_EQ(testTree.find(7).value()[0], 125);
  ASSERT_FALSE(testTree.find(9));
}

// TEST(BPlusTree, CorrectlyDeletesElementOnlyPresentInLeafNode) {
//     auto testTree = createTreeWithKeyAsValue<4, uint32_t>({0, 1, 2, 3, 4, 5,
//     6, 7, 8, 9, 10, 11, 12, 13, 14, 15});
//     ASSERT_TRUE(testTree.find(6).has_value());
//     testTree.remove(6);
//
//     // Test either side to ensure the surrounding records have not been
//     affected ASSERT_EQ(testTree.find(5), 5);
//     ASSERT_FALSE(testTree.find(6).has_value());
//     ASSERT_EQ(testTree.find(7), 7);
// }

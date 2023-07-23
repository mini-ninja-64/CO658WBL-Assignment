#pragma once

#include <filesystem>
#include <fstream>
#include <memory>

#include "database/file/Header.hpp"
#include "database/file/data/DataFile.hpp"
#include "database/file/index/IndexFile.hpp"
#include "database/file/index/tree/FileBackedNode.hpp"
#include "database/file/index/tree/friends.hpp"
#include "utils/string.hpp"

template <typename K, typename ADDRESS> class FileBackedBPlusTree {
private:
  IndexFile<K, ADDRESS> indexFile;
  DataFile<ADDRESS> dataFile;

private:
  std::tuple<LazyNode<K, ADDRESS>, LazyNode<K, ADDRESS>>
  findParentAndLeaf(const K &key) {
    auto root = indexFile.getRootNode();
    if (root.get()->getNodeType() == NodeType::Leaf) {
      return std::make_tuple(root, root);
    }

    auto parent = root;
    std::optional<LazyNode<K, ADDRESS>> leaf;
    do {
      LazyNode<K, ADDRESS> child =
          std::static_pointer_cast<FileBackedInternal<K, ADDRESS>>(parent.get())
              ->next(key);
      if (child.get()->getNodeType() == NodeType::Leaf)
        leaf = child;
      else
        parent = child;
    } while (!leaf);

    return std::make_tuple(parent, leaf.value());
  }

  std::string recurseChildrenGraphViz(LazyNode<K, ADDRESS> &node) {
    auto getNodeName = [](LazyNode<K, ADDRESS> lazyNode) -> std::string {
      return "node" + std::to_string(lazyNode.getAddress());
    };
    std::string graphVizCode;

    auto records = node.get()->getRecords();
    std::string labelText = "Records: [ " + joinString<K>(records, ", ") + " ]";

    if (node.get()->getNodeType() == NodeType::Leaf) {
      auto leafPointer =
          std::static_pointer_cast<FileBackedLeaf<K, ADDRESS>>(node.get());
      auto dataAddresses = leafPointer->getDataAddresses();
      labelText +=
          "\nValues: { " + joinString<ADDRESS>(dataAddresses, ", ") + " }";
      auto nextNode = leafPointer->getNextNode();
      if (nextNode)
        graphVizCode += getNodeName(node) + " -> " +
                        getNodeName(nextNode.value()) +
                        "[color=\"orange\"; constraint=false;]\n";
      auto previousNode = leafPointer->getPreviousNode();
      if (previousNode)
        graphVizCode += getNodeName(node) + " -> " +
                        getNodeName(previousNode.value()) +
                        "[color=\"green\"; constraint=false;]\n";
    }

    auto nodeName = "node" + std::to_string(node.getAddress());
    std::string labelledNode = nodeName + " [ label=\"" + labelText + "\" ];\n";
    graphVizCode += labelledNode;

    if (node.get()->getParent()) {
      auto parentNodeName =
          "node" + std::to_string(node.get()->getParent().value().getAddress());
      graphVizCode += nodeName + " -> " + parentNodeName + "[color=\"blue\"]\n";
    }

    if (node.get()->getNodeType() == NodeType::Internal) {
      auto internalNode =
          std::static_pointer_cast<FileBackedInternal<K, ADDRESS>>(node.get());
      for (const auto &childAddress : internalNode->getChildrenAddresses()) {
        LazyNode<K, ADDRESS> childNode(&indexFile, childAddress);
        graphVizCode += recurseChildrenGraphViz(childNode);
        std::string childNodeName = "node" + std::to_string(childAddress);
        graphVizCode += nodeName + " -> ";
        graphVizCode += childNodeName + "[color=\"red\"]\n";
      }
    }
    return graphVizCode;
  }

  void addChildToParent(LazyNode<K, ADDRESS> parentNode,
                        LazyNode<K, ADDRESS> childNode,
                        const K &propagatedKey) {
    if (parentNode.get()->getNodeType() != NodeType::Internal)
      throw std::domain_error("Only an internal node can be a parent node");
    auto parentNodePointer =
        std::static_pointer_cast<FileBackedInternal<K, ADDRESS>>(
            parentNode.get());
    auto childNodePointer = childNode.get();

    childNodePointer->setParentAddress(parentNode.getAddress());
    indexFile.saveNode(childNode.getAddress(), childNodePointer);

    parentNodePointer->addChild(propagatedKey, childNode.getAddress());
    indexFile.saveNode(parentNode.getAddress(), parentNodePointer);

    if (parentNodePointer->getRecords().size() >=
        indexFile.getMetadata().graphOrder) {
      const auto leftNodeSize = indexFile.getMetadata().graphOrder / 2;
      const auto keyToPropagate = parentNodePointer->getRecords()[leftNodeSize];
      auto newRightParent = splitRight(parentNode, leftNodeSize);
      auto nextParent = parentNodePointer->getParent();

      if (parentNode == getRoot()) {
        auto newRootPointer = std::make_unique<FileBackedInternal<K, ADDRESS>>(
            &indexFile, parentNode.getAddress(), keyToPropagate,
            newRightParent.getAddress());
        auto newRoot = indexFile.insertNode(std::move(newRootPointer));
        indexFile.setRootNode(newRoot.getAddress());

        // TODO: optimize
        parentNodePointer->setParentAddress(newRoot.getAddress());
        auto rightParentPointer = newRightParent.get();
        rightParentPointer->setParentAddress(newRoot.getAddress());

        indexFile.saveNode(parentNode.getAddress(),
                           std::move(parentNodePointer));
        indexFile.saveNode(newRightParent.getAddress(),
                           std::move(rightParentPointer));
      } else {
        addChildToParent(nextParent.value(), newRightParent, keyToPropagate);
      }
    }
  }

public:
  FileBackedBPlusTree(const std::filesystem::path &indexFilePath,
                      const std::filesystem::path &dataFilePath,
                      size_t defaultOrder, uint32_t defaultChunkSize,
                      bool forceOverwrite)
      : indexFile(indexFilePath, forceOverwrite, defaultOrder),
        dataFile(dataFilePath, forceOverwrite, defaultChunkSize) {
    indexFile.getRootNode().get();
  }

  void insert(const K &key, const std::vector<uint8_t> &data) {
    auto [parent, leaf] = findParentAndLeaf(key);

    const auto dataChunkSize = dataFile.getMetadata().dataChunkSize;
    // Round up the number of data chunks required to accommodate the provided
    // data
    auto numberOfChunksRequired = (data.size() + dataChunkSize) / dataChunkSize;
    ADDRESS firstDataChunk = 0;
    std::optional<ADDRESS> currentDataChunk = std::nullopt;
    for (std::vector<uint8_t>::size_type i = 0; i < numberOfChunksRequired;
         ++i) {
      auto dataOffset = std::min((i + 1) * dataChunkSize, data.size());
      std::vector<uint8_t> rawData = {data.begin() + (i * dataChunkSize),
                                      data.begin() + dataOffset};
      auto dataPointer =
          std::make_shared<DataChunk<ADDRESS>>(rawData, currentDataChunk);
      const auto newData = dataFile.insertData(std::move(dataPointer));
      if (firstDataChunk == 0)
        firstDataChunk = newData.getAddress();
      currentDataChunk = newData.getAddress();
    }

    auto leafPointer =
        std::static_pointer_cast<FileBackedLeaf<K, ADDRESS>>(leaf.get());
    leafPointer->insertOrdered(key, firstDataChunk);
    indexFile.saveNode(leaf.getAddress(), leafPointer);

    if (leafPointer->getRecords().size() >=
        indexFile.getMetadata().graphOrder) {
      const auto leftNodeSize = indexFile.getMetadata().graphOrder / 2;

      auto rightLeaf = splitRight(leaf, leftNodeSize);
      auto separationKey = rightLeaf.get()->getRecords()[0];

      if (leaf == indexFile.getRootNode()) {
        // This means we are splitting the root node as there is no parent,
        // the cleanest way to carry this out is to create a new leaf to the
        // right then replace the existing root which is a leaf, with a new
        // internal node that utilises the old root as a leaf child.
        auto newRootPointer = std::make_unique<FileBackedInternal<K, ADDRESS>>(
            &indexFile, leaf.getAddress(), separationKey,
            rightLeaf.getAddress());
        auto newRoot = indexFile.insertNode(std::move(newRootPointer));
        indexFile.setRootNode(newRoot.getAddress());

        // TODO: optimize
        leafPointer->setParentAddress(newRoot.getAddress());
        auto rightLeafPointer = rightLeaf.get();
        rightLeafPointer->setParentAddress(newRoot.getAddress());

        indexFile.saveNode(leaf.getAddress(), std::move(leafPointer));
        indexFile.saveNode(rightLeaf.getAddress(), std::move(rightLeafPointer));
      } else {
        // This means we must split the leaf to the right and carry out the
        // standard insertion procedure of a B+ Tree, which in this case is
        // handled recursively in the addChildToParent member function.
        addChildToParent(parent, rightLeaf, separationKey);
      }
    }
  }

  LazyNode<K, ADDRESS> getRoot() { return indexFile.getRootNode(); }

  [[nodiscard]] std::optional<std::vector<uint8_t>> find(K key) {
    auto [parent, leaf] = findParentAndLeaf(key);
    auto recordIndex = leaf.get()->indexOf(key);
    if (recordIndex) {
      auto leafNodePointer =
          std::static_pointer_cast<FileBackedLeaf<K, ADDRESS>>(leaf.get());
      std::optional<ADDRESS> currentDataAddress =
          leafNodePointer->getDataAddresses()[recordIndex.value()];
      std::vector<uint8_t> rawData;
      while (currentDataAddress) {
        auto dataChunk = dataFile.getData(currentDataAddress.value());
        currentDataAddress = dataChunk->getNextChunk();
        auto &dataChunkRawData = dataChunk->getData();
        rawData.insert(rawData.end(), dataChunkRawData.begin(),
                       dataChunkRawData.end());
      }
      return rawData;
    }
    return std::nullopt;
  }

  [[nodiscard]] std::string renderGraphVizView() {
    std::string graphVizDoc = "digraph G {\n";
    auto root = getRoot();
    graphVizDoc += recurseChildrenGraphViz(root);
    graphVizDoc += "}";
    return graphVizDoc;
  }

  [[nodiscard]] const IndexFile<K, ADDRESS> &getIndexFile() const {
    return indexFile;
  }
  [[nodiscard]] const DataFile<ADDRESS> &getDataFile() const {
    return dataFile;
  }
};

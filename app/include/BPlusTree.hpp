#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <cstddef>
#include <iostream>
#include <optional>


template<typename KEY_TYPE, typename VALUE_TYPE>
class Node {
private:
    Node<KEY_TYPE, VALUE_TYPE>* parent = nullptr;
    std::vector<KEY_TYPE> records = {};
    std::vector<Node<KEY_TYPE, VALUE_TYPE>*> children = {};
    std::vector<VALUE_TYPE> values = {};
    bool leaf = false;
public:
    Node(const Node&) = delete;
    Node() = default;
    Node(Node<KEY_TYPE, VALUE_TYPE>* parent,
         const std::vector<KEY_TYPE> &records,
         const std::vector<Node<KEY_TYPE, VALUE_TYPE>*>  &children,
         const std::vector<VALUE_TYPE> &values,
         bool leaf): parent(parent), records(records), children(children), values(values), leaf(leaf) {}

    [[nodiscard]] size_t numberOfChildren() const {
        return children.size();
    }

    [[nodiscard]] size_t numberOfRecords() const {
        return records.size();
    }

    Node<KEY_TYPE, VALUE_TYPE> *getParent() const {
        return parent;
    }

    [[nodiscard]] bool isLeaf() const {
        return leaf;
    }

    void setLeaf(bool isLeaf) {
        leaf = isLeaf;
    }

    std::vector<KEY_TYPE> &getRecords() {
        return records;
    }

    std::vector<Node<KEY_TYPE, VALUE_TYPE>*> &getChildren() {
        return children;
    }

    std::vector<VALUE_TYPE> &getValues() {
        return values;
    }

    Node<KEY_TYPE, VALUE_TYPE>* findLeaf(KEY_TYPE key) {
        if(children.empty() && leaf) return this;
        size_t nextChild = 0;
        for (const auto& record: records) {
            if(record > key) break;
            nextChild++;
        }
        return children[nextChild]->findLeaf(key);
    }

    std::tuple<Node<KEY_TYPE, VALUE_TYPE>*, Node<KEY_TYPE, VALUE_TYPE>*> findParentAndLeaf(KEY_TYPE key) {
        if(children.empty() && leaf) return std::make_tuple(nullptr, this);
        size_t nextChildIndex = 0;
        for (const auto& record: records) {
            if(record > key) break;
            nextChildIndex++;
        }

        auto& nextChild = children[nextChildIndex];
        if (!children.empty() && nextChild->isLeaf()) return std::make_tuple(this, nextChild);
        return nextChild->findParentAndLeaf(key);
    }

    void addChild(Node<KEY_TYPE, VALUE_TYPE>* child) {
        children.push_back(child);
        child->parent = this;
    }

    void addChild(size_t position, Node<KEY_TYPE, VALUE_TYPE>* child) {
        children.insert(children.begin() + position, child);
        child->parent = this;
    }
};

// https://en.wikipedia.org/wiki/B%2B_tree
// https://www.cs.usfca.edu/~galles/visualization/BPlusTree.html

// TREE_ORDER = order / branching factor
// Branching factor is the capacity of internal nodes, the maximum number of children

// For all nodes with children:
//  minimum children = minimum records + 1
//  maximum records = maximum records + 1

// Root Node (no children)
//  minimum records = 0
//  maximum records = TREE_ORDER - 1

// Root Node
//  minimum records = 2
//  maximum records = TREE_ORDER - 1

// Internal Node
//  minimum records = roundUp(TREE_ORDER/2) - 1
//  maximum records = TREE_ORDER - 1

// Leaf Node (no children)
//  minimum records = roundUp(TREE_ORDER/2) - 1
//  maximum records = TREE_ORDER - 1

template<size_t ORDER, typename K, typename V>
class BPlusTree {
private:
    Node<K, V> root{nullptr, {},{},{}, true};

public:
    void addChildToParent(Node<K, V>* parentNode, Node<K, V>* childNode, K& propagatedKey) {
        int insertIndex = 0;
        for (const auto& record: parentNode->getRecords()) {
            if(propagatedKey < record) break;
            insertIndex++;
        }
        parentNode->getRecords().insert(parentNode->getRecords().begin()+insertIndex, propagatedKey);
        parentNode->addChild(insertIndex+1,childNode);
        if(parentNode->numberOfRecords() >= ORDER) {
            size_t leftSize = ORDER/2;
//            size_t rightSize = ORDER-leftSize;

            if(parentNode == &root) { // parent is root, create 2 nodes left and right and push under root
                auto keyToPropogateUpwards = parentNode->getRecords()[leftSize];
                auto newLeftParent = new Node<K,V>(&root,
                                                   {root.getRecords().begin(), root.getRecords().begin() + leftSize},
                                                   {root.getChildren().begin(), root.getChildren().begin() + leftSize + 1},
                                                   {},
                                                   false);
                auto newRightParent = new Node<K,V>(&root,
                                                   {root.getRecords().begin()+leftSize+1, root.getRecords().end()},
                                                   {root.getChildren().begin()+leftSize+1, root.getChildren().end()},
                                                   {},
                                                   false);

                // TODO: wont work for all cases?
                root.getRecords().clear();
                root.getRecords().push_back(keyToPropogateUpwards);
                root.getChildren().clear();
                root.addChild(newLeftParent);
                root.addChild(newRightParent);
            } else {
                // create
                [[maybe_unused]]
                auto keyToPropagate = parentNode->getRecords()[leftSize];

                auto newRightParent = new Node<K,V>(parentNode->getParent(),
                                                    {parentNode->getRecords().begin()+leftSize+1, parentNode->getRecords().end()},
                                                    {parentNode->getChildren().begin()+leftSize+1, parentNode->getChildren().end()},
                                                    {},
                                                    false);
                parentNode->getRecords().erase(parentNode->getRecords().begin()+leftSize, parentNode->getRecords().end());
                parentNode->getChildren().erase(parentNode->getChildren().begin()+leftSize+1, parentNode->getChildren().end());
                addChildToParent(parentNode->getParent(), newRightParent, keyToPropagate);
            }
        }
    }

    void insert(K key, [[maybe_unused]] V value) {
        // Jump to the leaf
        auto [ parent, leaf ] = root.findParentAndLeaf(key);

        int insertIndex = 0;
        for (const auto& record: leaf->getRecords()) {
            if(key < record) break;
            insertIndex++;
        }
        leaf->getRecords().insert(leaf->getRecords().begin()+insertIndex, key);

        if(leaf->numberOfRecords() >= ORDER) {
            size_t leftSize = ORDER/2;

            if(parent == nullptr) { // root node
                auto leftLeaf = new Node<K, V>(&root, {leaf->getRecords().begin(), leaf->getRecords().begin() + leftSize}, {}, {}, true);
                auto rightLeaf = new Node<K, V>(&root, {leaf->getRecords().begin() + leftSize, leaf->getRecords().end()}, {}, {}, true);
                root.getRecords().clear();
                root.getRecords().push_back(rightLeaf->getRecords()[0]);
                root.addChild(leftLeaf);
                root.addChild(rightLeaf);
            } else { // normal case, split the leaf to the right
                auto newRightLeaf = new Node<K, V>(parent, {leaf->getRecords().begin() + leftSize, leaf->getRecords().end()}, {}, {}, true);
                auto keyToPropagate = leaf->getRecords()[leftSize];
                leaf->getRecords().erase(leaf->getRecords().begin() + leftSize, leaf->getRecords().end());
                addChildToParent(parent, newRightLeaf, keyToPropagate);
            }
        }
    }

    Node<K, V> &getRoot() {
        return root;
    }

    std::string generateGraphVizNode(const std::string& nodeName, Node<K,V>* node) {
        std::string graphVizNode = nodeName + " [ label=\"[";
        for (const auto &record: node->getRecords()) {
            graphVizNode += std::to_string(record) + ",";
        }
        graphVizNode += "]\" ];\n";

        return graphVizNode;
    }

    std::string renderGraphVizChunk(const std::string& nodeName, Node<K,V>* node, int& currentIndex) {
        std::string graphVizCode = generateGraphVizNode(nodeName, node);

        for (auto& child : node->getChildren()) {
            std::string childNodeName = "childNode" + std::to_string(currentIndex);
            currentIndex++;
            graphVizCode += renderGraphVizChunk(childNodeName, child, currentIndex);
            graphVizCode += nodeName + " -> ";
            graphVizCode += childNodeName + "\n";
        }
        return graphVizCode;
    }

    std::string renderGraphViz() {
        std::string graphVizDoc = "digraph G {\n";
        int nodeIndex = 0;
        graphVizDoc += renderGraphVizChunk("root", &root, nodeIndex);
        graphVizDoc += "}";
        return graphVizDoc;
    }
};

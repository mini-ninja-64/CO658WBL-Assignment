#pragma once

#include <cstdint>
#include <array>
#include <vector>
#include <cstddef>
#include <iostream>
#include <optional>
#include <tuple>
#include <span>

#include "utils/string.hpp"
#include "database/Node.hpp"
#include "Internal.hpp"
#include "Leaf.hpp"

// TODO: Obviously destructor
template<size_t ORDER, typename K, typename V>
class BPlusTree {
private:
    Node2<ORDER, K, V>* root = new Leaf<ORDER, K, V>();
    size_t nodeCount = 1;

    std::tuple<Internal<ORDER, K, V>*, Leaf<ORDER, K, V>*> findParentAndLeaf(const K& key) const {
        if(root->getNodeType() == NodeType::Leaf) {
            return std::make_tuple(nullptr, dynamic_cast<Leaf<ORDER, K, V>*>(root));
        }

        Internal<ORDER, K, V>* parent = dynamic_cast<Internal<ORDER, K, V>*>(root);
        Leaf<ORDER, K, V>* leaf = nullptr;
        do {
            auto child = parent->next(key);
            if(child->getNodeType() == NodeType::Leaf) leaf = dynamic_cast<Leaf<ORDER, K, V>*>(child);
            else parent = dynamic_cast<Internal<ORDER, K, V>*>(child);
        } while (leaf == nullptr);

        return std::make_tuple(parent, leaf);
    }

    std::string recurseChildrenGraphViz(const std::string& nodeName,
                                        Node2<ORDER, K,V>* node,
                                        int& currentIndex) const {
        auto records = node->getRecords();
        std::string labelText = "Records: [ " + joinString<K>(records, ", ") + " ]";
        if(node->getNodeType() == NodeType::Leaf) {
            auto values = dynamic_cast<Leaf<ORDER, K,V>*>(node)->getValues();
            labelText += "\nValues: { " + joinString<V>(values, ", ") + " }";
        }

        std::string labelledNode = nodeName + " [ label=\"" + labelText + "\" ];\n";
        std::string graphVizCode = labelledNode;

        if (node->getNodeType() == NodeType::Internal) {
            for (auto& child : dynamic_cast<Internal<ORDER, K,V>*>(node)->getChildren()) {
                std::string childNodeName = "childNode" + std::to_string(currentIndex);
                currentIndex++;
                graphVizCode += recurseChildrenGraphViz(childNodeName, child, currentIndex);
                graphVizCode += nodeName + " -> ";
                graphVizCode += childNodeName + "\n";
            }
        }
        return graphVizCode;
    }

    void addChildToParent(Internal<ORDER, K, V>* parentNode, Node2<ORDER, K, V>* childNode,const K& propagatedKey) {
        parentNode->addChild(propagatedKey, childNode);
        if(parentNode->getRecords().size() >= ORDER) {
            size_t leftSize = ORDER/2;

            if(parentNode == root) {
                auto keyToPropagate = parentNode->getRecords()[leftSize];
                auto newRightParent = parentNode->splitRight(leftSize);
                root = new Internal<ORDER, K, V>(root, keyToPropagate, newRightParent);
                nodeCount+=2;
            } else {
                auto keyToPropagate = parentNode->getRecords()[leftSize];
                auto newRightParent = parentNode->splitRight(leftSize);
                nodeCount+=1;
                auto nextParent = parentNode->getParent();
                if(nextParent->getNodeType() != NodeType::Internal)
                    throw std::invalid_argument("A node with an invalid parent type was supplied, parents can only be internal");
                addChildToParent(dynamic_cast<Internal<ORDER, K, V>*>(nextParent), newRightParent, keyToPropagate);
            }
        }
    }

public:
    [[nodiscard]] size_t getNodeCount() const {
        return nodeCount;
    }

    Node2<ORDER, K, V>* getRoot() const {
        return root;
    }


    void insert(const K& key, const V& value) {
        // Jump to the leaf
        auto [ parent, leaf ] = findParentAndLeaf(key);

        // TODO: dont rely on insertion to a full node
        leaf->insertOrdered(key, value);

        if(leaf->getRecords().size() >= ORDER) {
            size_t leftSize = ORDER/2;

            if(parent == nullptr) {
                // This means we are splitting the root node as there is no parent,
                // the cleanest way to carry this out is to create a new leaf to the right
                // then replace the existing root which is a leaf, with a new internal node
                // that utilises the old root as a leaf child.
                auto newLeaf = dynamic_cast<Leaf<ORDER, K, V>*>(root)->splitRight(leftSize);
                root = new Internal<ORDER, K, V>(root, newLeaf->getRecords()[0], newLeaf);
                nodeCount+=2;
            } else {
                // This means we must split the leaf to the right and carry out the standard
                // insertion procedure of a B+ Tree, which in this case is handled recursively
                // in the addChildToParent member function.
                auto newLeaf = leaf->splitRight(leftSize);
                nodeCount+=1;
                auto keyToPropagate = newLeaf->getRecords()[0];
                addChildToParent(parent, newLeaf, keyToPropagate);
            }
        }
    }

    [[nodiscard]] std::string renderGraphVizView() const {
        std::string graphVizDoc = "digraph G {\n";
        int nodeIndex = 0;
        graphVizDoc += recurseChildrenGraphViz("root", root, nodeIndex);
        graphVizDoc += "}";
        return graphVizDoc;
    }


    [[nodiscard]] std::optional<V> find(K key) const {
        auto [ parent, leaf ] = findParentAndLeaf(key);
        auto recordIndex = leaf->indexOf(key);
        if (recordIndex.has_value()) return leaf->getValues()[recordIndex.value()];
        return std::nullopt;
    }

    std::optional<V> remove(const K& key) {
        auto [ parent, leaf ] = findParentAndLeaf(key);

        // Handling Key Propagation
        // Note: Some B+ tree implementations choose to propagate upwards any key removals,
        //       this is an interesting choice and in my opinion does not serve much purpose,
        //       since the internal nodes are only used for searching, aligning a non-broken
        //       index won't improve search performance, but does cost budget to carry out.
        //       (at least with my current understanding)

        // Handling Tree Underflow
        // Note: Some B+ tree implementations (including this one) choose to ignore tree underflow's,
        //       the argument being that in actual systems the amount of inbound data far outweighs
        //       the number of deletions so anything deleted will likely be quickly replaced anyway.

        return leaf->remove(key);
    }
};

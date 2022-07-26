/**
 * @file node.h
 * @author dong
 * @brief 
 * @version 0.1
 * @date 2022-07-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef SKIPLIST_NODE_H
#define SKIPLIST_NODE_H

template<typename K, typename V>
class SkipList;

template<typename K, typename V>
class Node {
    friend class SkipList<K, V>;
private:
    /* data */
    K key_;
    V value_;
    Node<K, V> **pre_;
    int node_depth_;
public:
    Node() {}
    Node(K k_, V v_);
    ~Node();
    K get_key() const;
    V get_value() const;

};

template<typename K, typename V>
Node<K, V>::Node(const K k_, const V v_) {
    this->key_ = k_;
    this->value_ = v_;
}

template<typename K, typename V>
Node<K, V>::~Node() {
    delete[]pre_;
}

template<typename K, typename V>
K Node<K, V>::get_key() const {
    return this->key_;
}

template<typename K, typename V>
V Node<K, V>::get_value() const {
    return this->value_;
}
#endif //SKIPLIST_NODE_H
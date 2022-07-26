/**
 * @file skiplist.h
 * @author dong
 * @brief 
 * @version 0.1
 * @date 2022-07-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef SKIPLIST_SKIPLIST_H
#define SKIPLIST_SKIPLIST_H

#define DEBUG
#include<cstddef>
#include<cassert>
#include<ctime>
#include "node.h"
#include "utils.h"

using namespace std;

template<typename K, typename V>
class SkipList {
public:
    SkipList(K footer_key_) : rand_(0x12345678) {
        InitList(footer_key_);
    }
    ~SkipList() {
        FreeList();
    }
    // 不能声明为 Node
    Node<K, V> *SearchNode(K key_) const;
    bool InsertNode(K key_, V value_);
    bool RemoveNode(K key_, V &value);
    int SizeOfList() {
        return this.node_count_;
    }
    int get_list_depth() {
        return this.depth_;
    }
private:
    int depth_;
    Node<K, V> *footer_;
    Node<K, V> *header_;
    size_t node_count_;
    // the bigest depth
    static const int MAX_DEPTH = 16;
    Random rand_;

private:
    void InitList(K footer_key_);
    void FreeList();
    // *&表示指针的引用，**&表示指针的指针的引用
    void CreateNode(int depth_, Node<K, V> *&node_);
    void CreateNode(int depth_, Node<K, V> *&node_, K key_, V value_);
    int get_random_depth();
    // 转储
    void DumpAllNodes();
    void DumpOneNode(Node<K, V> *&node_, int node_depth_);
};

template<typename K, typename V>
void SkipList<K, V>::InitList(K footer_key_) {
    CreateNode(0, footer_);
    footer_->key_ = footer_key_;
    this->depth_ = 0;

    CreateNode(MAX_DEPTH, header_);
    for (int i = 0; i < MAX_DEPTH; ++i) {
        header_->pre_[i] = footer_;
    }
    this->node_count_ = 0;
}

template<typename K, typename V>
void SkipList<K, V>::CreateNode(int depth_, Node<K, V> *&node_) {
    node_ = new Node<K, V>(NULL, NULL);
    node_->pre_ = new Node<K, V> *[depth_ + 1];  // 0~depth
    node_->node_depth_ = depth_;
    assert(node_ != NULL);
}

template<typename K, typename V>
void SkipList<K, V>::CreateNode(int depth_, Node<K, V> *&node_, K key_, V value_) {
    node_ = new Node<K, V>(key_, value_);
    
}

#endif
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
        return this->node_count_;
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
        header_->next_[i] = footer_;
    }
    this->node_count_ = 0;
}

template<typename K, typename V>
void SkipList<K, V>::CreateNode(int depth_, Node<K, V> *&node_) {
    node_ = new Node<K, V>(NULL, NULL);
    node_->next_ = new Node<K, V> *[depth_ + 1];  // 0~depth
    node_->node_depth_ = depth_;
    assert(node_ != NULL);
}

template<typename K, typename V>
void SkipList<K, V>::CreateNode(int depth_, Node<K, V> *&node_, K key_, V value_) {
    node_ = new Node<K, V>(key_, value_);
    if (depth_ > 0) {
        node_->next_ = new Node<K, V> *[depth_ + 1];
    }
    node_->node_depth_ = depth_;
    assert(node_ != NULL);
}

template<typename K, typename V>
void SkipList<K, V>::FreeList() {
    Node<K, V> *p = this->header_;
    Node<K, V> *q;
    while (p != NULL) {
        q = p->next_[0];
        delete p;
        p = q;
    }
    delete p;
}

template<typename K, typename V>
Node<K, V>* SkipList<K, V>::SearchNode(const K key_) const{
    Node<K, V> *node_ = this->header_;
    for (int i = this->depth_; i >= 0; --i) {
        while ((node_->next_[i])->key_ < key_) {
            node_ = *(node_->next_ + i);
        }
    }
    node_ = node_->next_[0];
    if (node_->key_ == key_) {
        return node_;
    } else {
        return nullptr;
    }
}

template<typename K, typename V>
bool SkipList<K, V>::InsertNode(K key_, V value_) {
    Node<K, V> *next_update_[MAX_DEPTH]; // 要插入节点前的最近的节点
    Node<K, V> *node_ = header_;
    // find the node with the key_
    for (int i = this->depth_; i >= 0; --i) {
        while ((node_->next_[i])->key_ < key_) {
            node_ = *(node_->next_ + i);
        }
        next_update_[i] = node_;
    }
    // 首个节点插入时，node_->next_[0]就是footer
    node_ = node_->next_[0]; // node_->key_ >= key_
    // key_存在，只需要改value 或者直接返回错误
    if (node_->key_ == key_) {
        return false;
    }
    int new_node_depth_ = get_random_depth();
    if (new_node_depth_ > this->depth_) {
        ++this->depth_;
        new_node_depth_ = this->depth_;
        next_update_[new_node_depth_] = header_;
    }
    Node<K, V> *new_node_;
    CreateNode(new_node_depth_, new_node_, key_, value_);
    // 指向new node后面的指针指向new node，并且也需要更新new node指向的位置
    for (int i = new_node_depth_; i >= 0; --i) {
        node_ = next_update_[i];
        new_node_->next_[i] = node_->next_[i];
        node_->next_[i] = new_node_;
    }
    ++this->node_count_;

#ifdef DEBUG
    DumpAllNodes();
#endif

    return true;
};

template<typename K, typename V>
bool SkipList<K, V>::RemoveNode(K key_, V &value_) {
    Node<K, V> *next_update_[MAX_DEPTH]; // 要插入节点前的最近的节点
    Node<K, V> *node_ = header_;
    // find the node with the key_
    for (int i = this->depth_; i >= 0; --i) {
        while ((node_->next_[i])->key_ < key_) {
            node_ = *(node_->next_ + i);
        }
        next_update_[i] = node_;
    }
    // 首个节点插入时，node_->next_[0]就是footer
    node_ = node_->next_[0]; // node_->key_ >= key_
    // key_存在，只需要改value 或者直接返回错误
    if (node_->key_ == key_) {
        return false;
    }
    // 以上 delete 和 insert 相同
    value_ = node_->value_;
    // 指向 node 的的要指向 node 的next
    Node<K, V> *tmp_;
    for (int i = 0; i < this->depth_; ++i) {
        tmp_ = next_update_[i];
        if (tmp_->next_[i] != node_) {
            break;
        }
        tmp_->next_[i] = node_->next_[i];
    }
    delete node_;
    // update depth
    while (this->depth_ > 0 && this->header_->next_[this->depth_] == this->footer_) {
        --this->depth_;
    }
    --this->node_count_;

#ifdef DEBUG
    DumpAllNodes();
#endif

    return true;  
}

template<typename K, typename V>
void SkipList<K, V>::DumpAllNodes() {
    Node<K, V> *tmp_ = this->header_;
    while (tmp_->next_[0] != this->footer_) {
        tmp_ = tmp_->next_[0];
        DumpOneNode(tmp_, tmp_->node_depth_);
        cout << "----------------------------" << endl;
    }
    cout << endl;
};

template<typename K, typename V>
void SkipList<K, V>::DumpOneNode(Node<K, V> *&node_, int node_depth_) {
    if (node_ == nullptr) {
        return;
    }
    cout << "node->key:" << node_->key_ << ",node->value:" << node_->value_ << endl;
    // <=!!!
    for (int i = 0; i <= node_depth_; ++i) {
        cout << "next[" << i << "]:" << "key_:" << node_->next_[i]->key_ << ",value_:" << node_->next_[i]->value_ << endl;
    }
}

template<typename K, typename V>
int SkipList<K, V>::get_random_depth() {
    int depth_ = static_cast<int>(rand_.Uniform(MAX_DEPTH));
    if (depth_ == 0) {
        depth_ == 1;
    }
    return depth_;
}

#endif //SKIPLIST_SKIPLIST_H
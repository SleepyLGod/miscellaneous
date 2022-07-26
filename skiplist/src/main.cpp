/**
 * @file main.cpp
 * @author dong (2441164168lhd@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2022-07-26
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include<iostream>
#include"../skiplist.h"
using namespace std;

int main() {
    cout << "skiplist test now starts..." << endl;

    //给一个最大的整型值
    SkipList<int, int> skiplist(0x7fffffff);

    int length = 10;

    for (int i = 1; i <= length; ++i) {
        skiplist.InsertNode(i, i + 200);
    }

    cout << "The number of elements in skiplist is:" << skiplist.SizeOfList() << endl;

    if (skiplist.SizeOfList() != length) {
        cout << "insert failur." << endl;
    } else {
        cout << "insert success." << endl;
    }

    //测试查找
    int value = -1;
    int key = 9;
    Node<int, int> *searchResult = skiplist.SearchNode(key);
    if (searchResult != nullptr) {
        value = searchResult->get_value();
        cout << "search result for key " << key << ":" << value << endl;
    } else {
        cout << "search failure for key " << key << endl;
    }

    //重置value
    value = -1;

    //测试移除,测试不通过
    key = 6;
    cout<<endl<<"start to remove"<<endl;
    bool removeResult = skiplist.RemoveNode(key, value);
    if (removeResult) {
        cout << "removed node whose key is " << key << " and value is " << value << endl;
    } else {
        cout << "removed failure" << endl;
    }


    return 0;
}

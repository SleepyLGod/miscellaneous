/**
 * @file main.cpp
 * @author dong
 * @brief 
 * @version 0.1
 * @date 2022-07-27
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include<iostream>
#include<vector>
#include<chrono>
#include "thread_pool.h"

int main() {
    ThreadPool pool_instance(4);
    std::vector< std::future<int> > results;
    for (int i = 0; i < 8; ++i) {
        results.emplace_back(
            pool_instance.enqueue([i] {
                std::cout << "hello " << i << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                std::cout << "world " << i << std::endl;
                return i * i;
            })
        );
    }
    for (auto && result: results) { // 通过future.get()获取返回值
        std::cout << result.get() << ' ';
    }
    std::cout << std::endl;

    return 0;
}
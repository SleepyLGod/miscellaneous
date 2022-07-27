/**
 * @file thread_pool.h
 * @author dong
 * @brief 
 * @version 0.1
 * @date 2022-07-27
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>               // 数组
#include <queue>                // 队列
#include <memory>               // 内存操作
#include <thread>               // 线程相关
#include <mutex>                // 互斥量
#include <condition_variable>   // 条件变量
#include <future>               // 从异步获取结果
#include <functional>           // 包装函数为对象
#include <stdexcept>            // 异常相关

class ThreadPool {
public:
    ThreadPool(size_t);
    template<class F, class... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>;
    ~ThreadPool();
private:
    // need to keep track of threads so we can join them
    std::vector< std::thread > workers_;
    // the task queue
    std::queue< std::function<void()> > tasks_;

    // synchronization
    std::mutex queue_mutex_;
    std::condition_variable condition_;
    bool stop_; // 线程池运行(stop_ == false) or 停止(stop_ == true)
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool(size_t thread_number) : stop_(false) { // stop_ == false，所有线程都被阻塞挂起
    for(size_t i = 0; i < thread_number; ++i) {
        workers_.emplace_back( // 压入线程（task）
            [this] { // lamda表达式获取参数值方式
                for(;;) { // 无限循环作为线程
                    std::function< void() > task;
                    { // 大括号作用：临时变量的生存期，即控制lock的时间
                        std::unique_lock<std::mutex> lock(this->queue_mutex_);
                        this->condition_.wait(lock,
                            [this] {
                                return this->stop_ || !this->tasks_.empty(); //当stop==false(在运行) && tasks.empty(),该线程被阻塞
                            }); 
                        if (this->stop_ && this->tasks_.empty()) {
                            return; // 如果线程池运行或者任务列表不为空则继续后续操作，否则退出函数
                        }
                        task = std::move(this->tasks_.front()); // 移动构造函数
                        this->tasks_.pop();
                    }
                    task(); // 执行任务
                }
            }
        );
    }
}

// add new work item to the pool
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)  // && 引用限定符，参数的右值引用，  此处表示参数传入一个函数
    -> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;
    // packaged_task是对任务的一个抽象，我们可以给其传递一个函数来完成其构造。之后将任务投递给任何线程去完成，
    // 通过packaged_task.get_future()方法获取的future来获取任务完成后的产出值
    auto task = std::make_shared< std::packaged_task<return_type()> > ( // 指向F函数的智能指针
            std::bind(std::forward<F>(f), std::forward<Args>(args)...) // 传递函数进行构造
        );
    // future为期望，get_future获取任务完成后的产出值
    std::future<return_type> res = task->get_future(); // 获取future对象，如果task的状态不为ready，会阻塞当前调用者
    {
        std::unique_lock<std::mutex> lock(this->queue_mutex_); // 保持互斥性，避免多个线程同时运行一个任务

        // don't allow enqueueing after stopping the pool
        if(this->stop_) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }

        this->tasks_.emplace([task](){ 
            (*task)(); 
        });
        // 将task投递给线程去完成，vector尾部压入,
        // std::packaged_task 重载了 operator()，重载后的operator()执行function。
        // 因此可以(*task)()可以压入vector<function<void()>>
    }
    this->condition_.notify_one(); //选择一个wait状态的线程进行唤醒，并使他获得对象上的锁来完成任务(即其他线程无法访问对象)
    return res;
} // notify_one不能保证获得锁的线程真正需要锁，并且因此可能产生死锁

// the distructor joins all threads
inline ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex_);
        this->stop_ = true;
    }
    condition_.notify_all(); // 通知所有wait状态的线程竞争对象的控制权，唤醒所有线程执行
    for (std::thread &worker: workers_) { // for-each循环
        worker.join(); // 因为线程都开始竞争了，所以一定会执行完，join可等待线程执行完
    }
}

#endif
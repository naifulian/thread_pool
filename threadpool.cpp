#include <iostream>


#include "threadpool.h"

/*=============================ThreadPool 实现==================*/
const int TASK_MAX_THRESHOLD = 1024;

ThreadPool::ThreadPool()
    : m_initThreadSize(0), m_taskNum(0)
    , m_taskQueMaxThreshold(TASK_MAX_THRESHOLD)
    , m_poolMode(PoolMode::FIXED_MODE) 
{

}

ThreadPool::~ThreadPool() {

}

// 设置线程池工作模式
void ThreadPool::setPoolMode(PoolMode mode) {
    m_poolMode = mode;
}


// 设置任务队列的上限阈值
void ThreadPool::setTaskQueMaxThreshold(int threshold) {
    m_taskQueMaxThreshold = threshold;
}

// 向线程池提交任务，用户调用该接口传入一个可调用对象作为任务
void ThreadPool::submitTask(std::shared_ptr<Task> sp) {

}

 // 启动线程池
void ThreadPool::start(size_t initThreadSize) {
    if(initThreadSize < 0) {
        std::cout << "线程池初始线程数量不应为负数" << std::endl;
        return;
    }

    m_initThreadSize = initThreadSize;

    // 集中创建所有线程对象
    for(int i = 0; i < m_initThreadSize; ++i) {
        // 创建 thread 线程对象时，把线程函数给 thread 线程对象
        auto ptr = std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc, this));
        m_threads.emplace_back(ptr);
    }

    // 启动所有线程
    for (int i = 0; i < m_initThreadSize; ++i) {
        m_threads[i]->start();
    }
}

// 线程池的线程函数，从任务队列中消费任务
void ThreadPool::threadFunc() {
    std::cout << "Begin threadFunc, tid = " << std::this_thread::get_id() << std::endl;

    std::cout << "End threadFunc" << std::endl;
}

/*=======================Thread 实现======================*/

Thread::Thread(ThreadFunc func) : m_func(func) {

}

Thread::~Thread() {

}

// 启动线程，执行线程函数
void Thread::start() {
    // 常见一个线程来执行一个线程函数
    std::thread t(m_func);
    // 设置为分离线程
    t.detach();
}

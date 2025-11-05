#include <iostream>
#include <chrono>

#include "threadpool.h"

/*=============================ThreadPool 实现==================*/
const int TASK_MAX_THRESHOLD = 1024;                // 任务队列的最大阈值

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



 // 启动线程池
void ThreadPool::start(int initThreadSize) {
    // 检查参数有效性
    if (initThreadSize <= 0) {
        std::cout << "线程池初始线程数量不能小于等于 0" << std::endl;
        return;
    }
    
    m_initThreadSize = initThreadSize;

    // 集中创建所有线程对象
    for(size_t i = 0; i < m_initThreadSize; ++i) {
        // 创建 thread 线程对象时，把线程函数给 thread 线程对象
        m_threads.emplace_back(std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc, this)));
    }

    // 启动所有线程
    for (size_t i = 0; i < m_initThreadSize; ++i) {
        m_threads[i]->start();
    }
}

// 向线程池提交任务，用户调用该接口传入一个可调用对象作为任务
void ThreadPool::submitTask(std::shared_ptr<Task> sp) {
    // 1.获取任务队列的锁
    std::unique_lock<std::mutex> lock(m_mtx_taskQue);

    // 2.等待任务队列有空余，最长时间不能超过 1s，否则判断此次提交任务失败
    /*while(m_taskQue.size() == m_taskQueMaxThreshold) {
        m_notFull.wait(lock);
    }*/
    if(!m_notFull.wait_for(lock, std::chrono::seconds(1),
        [&]()->bool { return m_taskQue.size() < static_cast<size_t>(m_taskQueMaxThreshold);})) {
        // 表示 m_notFull 条件变量等待了 1 秒之后还没有满足条件
        std::cerr << "task queue is full, submit task failed." << std::endl;
        return;
    }

    // 3.任务队列有空余，将任务放入任务队列中
    m_taskQue.emplace(sp);
    m_taskNum++;
    
    // 4. 提交新任务，在 notEmpty 信号量通知分配线程处理任务
    m_notEmpty.notify_all();
}

// 线程池的线程函数，从任务队列中消费任务
void ThreadPool::threadFunc() {

    while(1) {
        std::shared_ptr<Task> task;
        {
            // 1.先获取任务队列的锁
            std::unique_lock<std::mutex> lock(m_mtx_taskQue);

            // 2.等待 m_notEmpty 条件变量
            m_notEmpty.wait(lock, [&]() -> bool {
                return m_taskQue.size() > 0;
            });

            // 3.如果任务队列不空，则从任务队列中取一个任务
            task = m_taskQue.front();
            m_taskQue.pop();
            m_taskNum--;

            // 4.如果依然有剩余任务，则通知其他线程执行任务
            if(m_taskQue.size() > 0) {
                m_notEmpty.notify_all();
            }

            // 5.取出一个任务，通知可以继续提交任务
            m_notFull.notify_all();
        } // 6.释放掉锁

        // 7.当前线程负责执行这个任务
        if(task != nullptr) {
            task->run();
        }
    }
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

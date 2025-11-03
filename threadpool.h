#ifndef __THREAD_POOL__
#define __THREAD_POOL__

#include <vector>
#include <queue>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>

const int DEFAULT_THREAD_NUM = 4;

// 线程池的模式
enum class PoolMode {
    FIXED_MODE,         // 固定数量的线程
    CACHED_MODE         // 线程数量可以动态增长
};

// 任务(抽象基类)，用户可以自定义任何任务类型，继承 Task 类，
// 然后重写 run 方法
class Task {
public:
    virtual void run() = 0;

private:
};

// fixed 模式不需要考虑线程安全，因为线程池对象初始化直接创建好了线程
// cached 模式需要考虑线程安全，线程回收可能要在不同的线程对线程队列删除

// 对于任务队列，需要使用多态，所以使用指针，为了保证任务队列的生命周期
// (考虑用户提交任务是否传进来一个临时对象)，需要使用智能指针

// 记录任务个数的成员变量需要考虑线程安全问题，使用原子类型就行了 => 为什么需要单独一个变量记录任务数量?

// 线程池不需要拷贝构造和赋值 -> why：包含不可拷贝资源
class ThreadPool {
public:
    ThreadPool();
    ~ThreadPool();

    ThreadPool(const ThreadPool &) = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;
    // 启动线程池
    void start(size_t initThreadSize = DEFAULT_THREAD_NUM);
    // 设置线程池工作模式
    void setPoolMode(PoolMode mode);

    // 设置任务队列的上限阈值
    void setTaskQueMaxThreshold(int threshold);
    // 向线程池提交任务
    void submitTask(std::shared_ptr<Task> sp);
private:
    std::vector<Thread*> m_thread;                      // 线程列表
    size_t m_initThreadSize;                            // 初始的线程数量

    std::queue<std::shared_ptr<Task>> m_taskQue;        // 任务队列
    std::atomic_uint m_taskNum;                         // 任务数量
    int m_taskQueMaxThreshold;                          // 任务队列数量上限的阈值

    std::mutex m_mtx_taskQue;                           // 任务队列的互斥锁，保证任务队列的线程安全
    std::condition_variable m_notFull;                  // 表示任务队列不满，向生产者发出消息
    std::condition_variable m_notEmpty;                 // 表示任务队列不空，

    PoolMode m_poolMode;                                // 当前线程池的工作模式
};

class Thread {
public:

private:
};

#endif 
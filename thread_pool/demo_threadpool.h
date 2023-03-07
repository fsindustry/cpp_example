//
// Created by fsindustry on 2023/3/7.
//

#ifndef CPP_EXAMPLE_DEMO_THREADPOOL_H
#define CPP_EXAMPLE_DEMO_THREADPOOL_H

#include <functional>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <vector>
#include <future>

void getNow(timeval *tv);

int64_t getNowMs();

#define TNOW      getNow()
#define TNOWMS    getNowMs()

class demo_threadpool {
protected:

  /**
   * 定义线程池运行任务
   */
  struct task_func {
    std::function<void()> _func;
    int64_t _expire_time;

    task_func(int64_t expire_time) : _expire_time(expire_time) {}
  };

  typedef std::shared_ptr<task_func> task_func_ptr;

  /**
   * 任务队列
   */
  std::queue<task_func_ptr> _tasks;

  /**
   * 工作线程
   */
  std::vector<std::thread *> _workers;

  /**
   * 线程池锁
   */
  std::mutex _pool_mutex;

  /**
   * 线程池条件变量
   */
  std::condition_variable _pool_cond;

  /**
   * 线程池大小
   */
  size_t _pool_size;

  /**
   * 是否终止线程池运行
   */
  bool _terminate;

  /**
   * 线程任务计数器
   */
  std::atomic<int> running_count{0};

public:
  demo_threadpool();

  virtual ~demo_threadpool();

  /**
   * 初始化线程池参数
   *
   * @param num 工作线程个数
   */
  bool initial(size_t num);

  /**
   * 启动线程池
   */
  bool start();

  /**
   * 关闭线程池
   */
  void stop();


  /**
   * 获取线程池中线程总数
   */
  size_t get_pool_size() {
    std::unique_lock<std::mutex> lock(_pool_mutex);
    return _pool_size;
  }

  /**
   * 获取当前等待运行的任务数
   */
  size_t get_pending_task() {
    std::unique_lock<std::mutex> lock(_pool_mutex);
    return _tasks.size();
  }

  /**
   * 向线程池提交任务
   * @param f 线程池待执行任务
   * @param args 任务返回值
   * @return future对象
   */
  template<class F, class... Args>
  auto exec(F &&f, Args &&... args) -> std::future<decltype(f(args...))> {
    // 递归展开可变参数
    return exec(0, f, args...);
  }

  /**
   * 向线程池提交任务（带超时时间）
   * @param timeout_ms 任务超时时间（毫秒）
   * @param f 线程池待执行任务
   * @param args 任务返回值
   * @return future对象
   */
  template<class F, class... Args>
  auto exec(int64_t timeout_ms, F &&f, Args &&... args) -> std::future<decltype(f(args...))> {
    // 计算超时时间，0表示不超时
    int64_t expire_time = timeout_ms == 0 ? 0 : TNOWMS + timeout_ms;
    using return_type = decltype(f(args...)); // 推导返回值

    // 封装任务
    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    task_func_ptr task_ptr = std::make_shared<task_func>(expire_time);
    task_ptr->_func = [task]() {
      (*task)(); // 调用封装的可执行对象
    };

    std::unique_lock<std::mutex> lock(_pool_mutex);
    // 添加任务到等待队列
    _tasks.push(task_ptr);
    // 唤醒线程池中的线程，执行任务
    _pool_cond.notify_one();
    return task->get_future();
  }

  /**
   * 等待任务队列中所有任务都执行完成
   * @param timeout_ms 等待超时时间，-1表示不超时
   * @return true, 所有任务都执行完成； false，超时退出
   */
  bool wait_for_all_done(int timeout_ms = -1);

protected:

  /**
   * 获取任务
   * @param task
   */
  bool get(task_func_ptr &task);

  /**
   * 线程池是否退出
   */
  bool is_terminate() const {
    return _terminate;
  }

  /**
   * 线程运行入口
   */
  void run();
};


#endif //CPP_EXAMPLE_DEMO_THREADPOOL_H

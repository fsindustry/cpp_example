//
// Created by fsindustry on 2023/3/7.
//

#include <sys/time.h>
#include "demo_threadpool.h"

demo_threadpool::demo_threadpool() : _pool_size(1), _terminate(false) {}

demo_threadpool::~demo_threadpool() {
  stop();
}

bool demo_threadpool::initial(size_t num) {
  std::unique_lock<std::mutex> lock(_pool_mutex);
  if (!_workers.empty()) {
    return false;
  }

  _pool_size = num;
  return true;
}

bool demo_threadpool::start() {

  std::unique_lock<std::mutex> lock(_pool_mutex);

  if (!_workers.empty()) {
    return false;
  }

  for (size_t i = 0; i < _pool_size; i++) {
    _workers.push_back(new std::thread(&demo_threadpool::run, this));
  }

  return false;
}

void demo_threadpool::stop() {

  {
    std::unique_lock<std::mutex> lock(_pool_mutex);
    _terminate = true;
    _pool_cond.notify_all();
  }

  for (auto t: _workers) {
    if (t->joinable()) {
      t->join();
    }
    delete t;
  }

  std::unique_lock<std::mutex> lock(_pool_mutex);
  _workers.clear();
}

bool demo_threadpool::wait_for_all_done(int timeout_ms) {

  std::unique_lock<std::mutex> lock(_pool_mutex);
  if (_tasks.empty()) {
    return true;
  }

  if (timeout_ms < 0) {
    _pool_cond.wait(lock, [this] { return _tasks.empty(); });
    return true;
  } else {
    return _pool_cond.wait_for(lock, std::chrono::milliseconds(timeout_ms), [this] { return _tasks.empty(); });
  }
}

bool demo_threadpool::get(demo_threadpool::task_func_ptr &task) {

  std::unique_lock<std::mutex> lock(_pool_mutex);

  if (_tasks.empty()) {
    _pool_cond.wait(lock, [this] { return _terminate || !_tasks.empty(); });
  }

  if (_terminate) {
    return false;
  }

  if (!_tasks.empty()) {
    task = std::move(_tasks.front());
    _tasks.pop();
    return true;
  }

  return false;
}

void demo_threadpool::run() {
  while (!is_terminate()) {
    task_func_ptr task;
    // 获取任务，若没有，则进入阻塞状态
    if (get(task)) {
      ++running_count;
      try {
        if (task->_expire_time != 0 && task->_expire_time < TNOWMS) {
          // 任务超时
        } else {
          // 执行任务
          task->_func();
        }
      } catch (...) {
      }

      --running_count;

      std::unique_lock<std::mutex> lock(_pool_mutex);
      if (running_count == 0 && _tasks.empty()) {
        // 通知wait_for_all_done()
        _pool_cond.notify_all();
      }
    }
  }
}


void getNow(timeval *tv) {
  gettimeofday(tv, DST_NONE);
}

int64_t getNowMs() {
  struct timeval tv;
  getNow(&tv);

  return tv.tv_sec * (int64_t) 1000 + tv.tv_usec / 1000;
}
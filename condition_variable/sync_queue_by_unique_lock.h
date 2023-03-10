//
// Created by fsindustry on 2023/3/5.
//

#ifndef CPP_EXAMPLE_SYNC_QUEUE_BY_UNIQUE_LOCK_H
#define CPP_EXAMPLE_SYNC_QUEUE_BY_UNIQUE_LOCK_H

#include <thread>
#include <condition_variable>
#include <mutex>
#include <list>
#include <iostream>

template<typename T>
class SimpleSyncQueue {
public:
  SimpleSyncQueue() {}

  void Put(const T &x) {
    std::lock_guard<std::mutex> locker(_mutex);
    _queue.push_back(x);
    _notEmpty.notify_one();
  }

  void Take(T &x) {
    std::unique_lock<std::mutex> locker(_mutex);
    _notEmpty.wait(locker, [this] { return !_queue.empty(); });

    x = _queue.front();
    _queue.pop_front();
  }

  bool Empty() {
    std::lock_guard<std::mutex> locker(_mutex);
    return _queue.empty();
  }

  size_t Size() {
    std::lock_guard<std::mutex> locker(_mutex);
    return _queue.size();
  }

private:
  std::list<T> _queue;
  std::mutex _mutex;
  std::condition_variable _notEmpty;
};


#endif //CPP_EXAMPLE_SYNC_QUEUE_BY_UNIQUE_LOCK_H

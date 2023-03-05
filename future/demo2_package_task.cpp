//
// Created by fsindustry on 2023/3/5.
//
#include <iostream>
#include <future>

using namespace std;

int add(int a, int b, int c) {
  std::cout << "call add\n";
  return a + b + c;
}

void do_other_things() {
  std::cout << "do_other_things" << std::endl;
}

int main() {
  std::packaged_task<int(int, int, int)> task(add);  // 封装任务
  do_other_things();
  std::future<int> result = task.get_future();
  task(1, 1, 2); //必须要让任务执行，否则在get()获取future的值时会一直阻塞
  std::cout << "result:" << result.get() << std::endl;
  return 0;
}

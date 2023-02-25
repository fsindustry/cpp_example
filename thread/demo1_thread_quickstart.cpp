//
// Created by fsindustry on 2023/2/6.
//
#include <iostream>
#include <thread>

using namespace std;

void thread_fun1(int &a) {
  cout << "this is thread_fun1, a + 10 = " << (a += 10) << endl;
}

void test1() {
  int a = 10;
  // std::ref用于传递参数引用
  std::thread t1(thread_fun1, std::ref(a));
  // std::move 切换线程控制权
  std::thread t2(std::move(t1));
  std::thread t3;
  t3 = std::move(t2);
  // 当前线程等待t3执行完成
  t3.join();
  cout << "main thread, a = " << a << endl;
}

int main() {
  test1();
}

//
// Created by fsindustry on 2023/2/6.
//
#include <iostream>
#include <thread>

using namespace std;

// 1）不传参数
void func1() {
  cout << "func1 into" << endl;
}

// 2）传递值类型参数
void func2(int a, int b) {
  cout << "func2 a + b = " << a + b << endl;
}

// 3）传递引用类型参数
void func3(int &c) {
  cout << "func3 c = " << &c << endl;
  c += 10;
}

// 4）传入类函数
class A {
public:
  void func4(int a) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    cout << "thread:" << name_ << ", fun4 a = " << a << endl;
  }

  void setName(string name) {
    name_ = name;
  }

  void displayName() {
    cout << "this:" << this << ", name:" << name_ << endl;
  }

  void play() {
    std::cout << "play call!" << std::endl;
  }

private:
  string name_;
};

// 5）detach
void func5() {
  cout << "func5 into sleep " << endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  cout << "func5 leave " << endl;
}

// 6）std::move
void func6() {
  cout << "this is func6 !" << endl;
}


int main() {
  // 1）不传参数
  cout << "\n\n main1--------------------------\n";
  std::thread t1(&func1); // 只传递函数
  t1.join(); // 阻塞等待线程函数执行结束

  // 2）传递值类型参数
  cout << "\n\n main2--------------------------\n";
  int a = 10;
  int b = 20;
  std::thread t2(func2, a, b); // 加上参数传递,可以任意参数
  t2.join();

  // 3）传递引用类型参数
  cout << "\n\n main3--------------------------\n";
  int c = 10;
  std::thread t3(func3, std::ref(c)); // 加上参数传递,可以任意参数
  t3.join();
  cout << "main3 c = " << &c << ", " << c << endl;

  // 4）传入类函数
  cout << "\n\n main4--------------------------\n";
  A *a4_ptr = new A();
  a4_ptr->setName("darren");
  std::thread t4(&A::func4, a4_ptr, 10);
  t4.join();
  delete a4_ptr;

  // 5）detach
  cout << "\n\n main5--------------------------\n";
  std::thread t5(&func5); // 只传递函数
  t5.detach(); // 脱离
  // 如果这里不休眠会怎么样
  std::this_thread::sleep_for(std::chrono::seconds(2));
  cout << "\n main5 end\n";

  // 6）std::move
  cout << "\n\n main6--------------------------\n";
  int x = 10;
  thread t6_1(func6);
  thread t6_2(std::move(t6_1)); // t6_1 线程失去所有权
  //  t6_1.join(); // 抛出异常
  t6_2.join();
  return 0;
}


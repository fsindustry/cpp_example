//
// Created by fsindustry on 2023/3/10.
//
#include<exception>
#include<iostream>

using namespace std;

// 自定义异常
class my_exception : public exception {
public:
  const char *what() const throw() { //throw () 表示不允许任何异常产生
    return "ERROR! Don't divide a number by integer zero.\n";
  }
};

void check(int y) throw(my_exception) { // 表示只允许抛出my_exception异常
  // 抛出自定义异常
  if (y == 0) throw my_exception();
}

int main() {
  int x = 100, y = 0;
  try {
    check(y);
    cout << x / y;
  } catch (my_exception &me) { // 捕获自定义异常
    cout << me.what();
    cout << "finish exception\n";
    return -1;
  }
  cout << "finish ok\n";
  return 0;
}

//
// Created by fsindustry on 2023/3/10.
//
#include <stdexcept> // 标准异常头文件
#include <limits>
#include <iostream>

using namespace std;

void MyFunc(int c) {
  if (c > numeric_limits<char>::max())
    // 抛出异常
    throw invalid_argument("throw MyFunc argument too large.");
}

int main() {
  try {
    MyFunc(256); //cause an exception to throw
  }
  catch (invalid_argument &e) { // 捕获异常
    cerr << "catch " << e.what() << endl;
    return -1;
  }
  cout << "end\n";
  return 0;
}

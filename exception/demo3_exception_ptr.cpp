//
// Created by fsindustry on 2023/3/10.
//
#include <iostream>       // std::cout
#include <exception>      // std::exception_ptr, std::current_exception, std::rethrow_exception
#include <stdexcept>      // std::logic_error

int main() {
  std::exception_ptr p;
  try {
    throw std::logic_error("some logic_error exception");   // throws
  } catch (const std::exception &e) {
    p = std::current_exception(); // 获取当前捕获的异常对象
    std::cout << "exception caught, but continuing...\n";
  }

  std::cout << "(after exception)\n";

  try {
    std::rethrow_exception(p); // 重新抛出指向的异常对象
  } catch (const std::exception &e) {
    std::cout << "exception caught: " << e.what() << '\n';
  }

  // 通过std::make_exception_ptr创建std::exception_ptr
  p = std::make_exception_ptr(std::logic_error("logic_error"));

  try {
    std::rethrow_exception (p);  // 重新抛出异常
  } catch (const std::exception& e) {
    std::cout << "exception caught: " << e.what() << '\n'; // 捕获异常
  }

  return 0;
}

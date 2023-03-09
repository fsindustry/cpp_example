//
// Created by fsindustry on 2023/3/10.
//
#include <iostream>       // std::cerr
#include <exception>      // std::exception, std::throw_with_nested, std::rethrow_if_nested
#include <stdexcept>      // std::logic_error

// recursively print exception whats:
void print_what(const std::exception &e) {
  std::cout << __FUNCTION__ << ", L" << __LINE__ << ", what:" << e.what() << '\n';
  try {
    std::rethrow_if_nested(e); // 重新抛出嵌套的下一层异常
  } catch (const std::exception &nested) {
    std::cerr << "nested: ";
    print_what(nested); // 递归解下一层异常
  }
}

// throws an exception nested in another:
void throw_nested() {
  try {
    throw std::logic_error("first");
  } catch (const std::exception &e) {
    std::throw_with_nested(std::logic_error("second")); // 抛出嵌套异常
  }
}

int main() {
  try {
    std::cout << __FUNCTION__ << ", L" << __LINE__ << std::endl;
    throw_nested();
  } catch (std::exception &e) {
    std::cout << __FUNCTION__ << ", L" << __LINE__ << std::endl;
    print_what(e);
  }

  return 0;
}

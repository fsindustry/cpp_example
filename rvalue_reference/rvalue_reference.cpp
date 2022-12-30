//
// Created by fsindustry on 2022/12/22.
//
#include <memory>
#include <iostream>

// 形参是个右值引用
void change(int &&right_value) {
    right_value = 8;
}

void demo_right_ref() {
    int a = 5; // a是个左值
    int &ref_a_left = a; // ref_a_left是个左值引用
    int &&ref_a_right = std::move(a); // ref_a_right是个右值引用
    // change(a); // 编译不过，a是左值，change参数要求右值
    // change(ref_a_left); // 编译不过，左值引用ref_a_left本身也是个左值
    // change(ref_a_right); // 编译不过，右值引用ref_a_right本身也是个左值
    change(std::move(a)); // 编译通过
    change(std::move(ref_a_right)); // 编译通过
    change(std::move(ref_a_left)); // 编译通过
    change(5); // 当然可以直接接右值，编译通过
    // 打印这三个左值的地址，都是一样的
    std::cout << &a << ' ';
    std::cout << &ref_a_left << ' ';
    std::cout << &ref_a_right;
}

int main() {
    int temp = 5;
    int &&ref_a = std::move(temp);
    ref_a = 6;
    std::cout << temp << std::endl;
}
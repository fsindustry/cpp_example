//
// Created by fsindustry on 2023/1/6.
//
#include <iostream>
#include <memory>

void test1() {
    // 有返回值的lambda表达式
    // [捕获列表](参数列表)->返回类型{函数体}
    auto Add = [](int a, int b) -> int {
        return a + b;
    };
    std::cout << Add(1, 2) << std::endl;
}

void test2() {
    // 编译器可以自动推断返回值类型，故返回值类型可省略
    // [捕获列表](参数列表){函数体}
    auto Add = [](int a, int b) {
        return a + b;
    };
    std::cout << Add(1, 2) << std::endl;
}

// 值捕获
void test3() {
    int c = 12;
    int d = 30;
    auto Add = [c, d]() -> int {
        std::cout << "c=" << c << ", d=" << d << std::endl;
        return c + d;
    };
    // 值传递，在创建lambda时，就已拷贝参数，故改变无效
    c = 5;
    d = 20;
    std::cout << Add() << std::endl; // 输出42
}

// 引用捕获
void test4() {
    int c = 12;
    int d = 30;
    auto Add = [&c, &d]() -> int {
        std::cout << "c=" << c << ", d=" << d << std::endl;
        return c + d;
    };
    // 引用传递，在创建lambda时，只传递引用，且lambda未执行，故此处改变有效；
    c = 5;
    d = 20;
    std::cout << Add() << std::endl; // 输出25
}

// 隐式捕获
void test5() {
    int c = 12;
    int d = 30;
    //auto Add = [=]() -> int { // 值捕获
    auto Add = [&]() -> int { // 引用捕获
        std::cout << "c=" << c << ", d=" << d << std::endl;
        return c + d;
    };
    // 引用传递，在创建lambda时，只传递引用，且lambda未执行，故此处改变有效；
    c = 5;
    d = 20;
    std::cout << Add() << std::endl; // 输出25
}

// 空捕获
void test6() {
    int c = 12;
    int d = 30;
    auto Add = [](int a, int b) -> int {
        std::cout << "a=" << a << ", b=" << b << std::endl;
        return a + b;
    };
    std::cout << Add(c, d) << std::endl; // 输出42
}

// 表达式捕获
void test7() {
    auto important = std::make_unique<int>(4);
    auto Add = [v1 = 3, v2 = std::move(important)](int a, int b) -> int {
        return a + b + v1 + *v2;
    };
    if (!important) {
        std::cout << "moved" << std::endl;
    }
    std::cout << Add(1, 2) << std::endl;
}

// 泛型lambda
void test8() {
    auto Add = [](auto a, auto b) {
        return a + b;
    };
    std::cout << Add(1, 2) << std::endl;
    std::cout << Add(1.2, 2.3) << std::endl;
}

// 可变lambda
void test9() {
    int v = 5;
    // 值捕获方式，使用mutable修饰，可以改变捕获的变量值
    auto l1 = [v]() mutable { return ++v; };
    v = 0; // 值传递，此处修改无效
    std::cout << l1() << std::endl; // 输出6

    // 引用捕获方式，无需mutable修饰，可以改变捕获的变量值
    auto l2 = [&v]() { return ++v; };
    v = 0; // 引用传递，此处修改有效
    std::cout << l2() << std::endl; // 输出1
}

int main() {
//    test1();
//    test2();
//    test3();
//    test4();
//    test5();
//    test6();
//    test7();
//    test8();
    test9();
}
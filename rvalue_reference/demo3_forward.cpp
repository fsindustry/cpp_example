//
// Created by fsindustry on 2023/1/5.
//
#include <iostream>

using namespace std;

template<class T>
void Print(T &t) {
    cout << "L" << t << endl;
}

template<class T>
void Print(T &&t) {
    cout << "R" << t << endl;
}

template<class T>
void func(T &&t) { // 右值引用即可以接收左值，也可以接收右值，但引用本身是左值；
    Print(t); // 右值引用也是左值
    Print(std::move(t)); // 左值转换为右值
    Print(std::forward<T>(t)); // 引用的值是左值，则转为左值；引用的值是右值，则转为右值；
}

int main() {
    cout << "-- func(1)" << endl;
    func(1); // 1为右值
    int x = 10;
    int y = 20;
    cout << "-- func(x)" << endl;
    func(x); // x本身是左值
    cout << "-- func(std::forward<int>(y))" << endl;
    func(std::forward<int>(y)); // y本身是左值，但其值是右值，故转为右值
    return 0;
}
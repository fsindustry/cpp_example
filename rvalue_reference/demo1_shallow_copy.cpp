//
// Created by fsindustry on 2022/12/29.
//
#include <iostream>

class A {
public:
    A(int *v) : m_ptr(v) {
        std::cout << "constructor A(" << *m_ptr << ")" << std::endl;
    }

    ~A() {
        std::cout << "destructor A, m_ptr:" << m_ptr << ", v: " << *m_ptr << std::endl;
        delete m_ptr;
        m_ptr = nullptr;
    }

    int *m_ptr;
};

// 为了避免返回值优化，此函数故意这样写
A Get(bool flag) {
    A a(new int(1));
    A b(new int(2));
    std::cout << "ready return" << std::endl;
    if (flag)
        return a;
    else {
        std::cout << "come here" << std::endl;
        return b;
    }
}

int main() {
    {
        // 拷贝过程中，没有重写拷贝构造函数，故编译器自动生成拷贝构造函数，并为m_ptr赋0值
        A c = Get(false); // 运行报错
        std::cout << *c.m_ptr << std::endl;
    }
    std::cout << "main finish" << std::endl;
    return 0;
}
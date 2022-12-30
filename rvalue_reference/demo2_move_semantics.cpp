//
// Created by fsindustry on 2022/12/29.
//
#include <iostream>
#include <vector>
#include <cstdlib>
#include <cstring>

using namespace std;


class MyString {
private:
    char *m_data;
    size_t m_len;

    void copy_data(const char *s) {
        m_data = new char[m_len + 1];
        memcpy(m_data, s, m_len);
        m_data[m_len] = '\0';
    }

public:
    /**
     * 默认构造函数
     */
    MyString() {
        m_data = nullptr;
        m_len = 0;
        std::cout << "Default Constructor is called! " << std::endl;
    }

    /**
     * 普通构造函数
     */
    MyString(const char *p) {
        m_len = strlen(p);
        copy_data(p);
        std::cout << "Ordinary Constructor is called! source: " << m_data << std::endl;
    }

    /**
     * 拷贝构造函数
     */
    MyString(const MyString &str) {
        m_len = str.m_len;
        copy_data(str.m_data);
        std::cout << "Copy Constructor is called! source: " << str.m_data << std::endl;
    }

    /**
     * 拷贝赋值运算符
     */
    MyString &operator=(const MyString &str) {
        if (this != &str) {
            m_len = str.m_len;
            copy_data(str.m_data);
        }
        std::cout << "Copy Assignment is called! source: " << str.m_data << std::endl;
        return *this;
    }

    /**
     * 移动构造函数
     */
    MyString(MyString &&str) {
        std::cout << "Move Constructor is called! source: " << str.m_data << std::endl;
        m_len = str.m_len;
        m_data = str.m_data; //避免了不必要的拷贝
        str.m_len = 0;
        str.m_data = nullptr;
    }

    /**
     * 移动赋值运算符
     */
    MyString &operator=(MyString &&str) {
        std::cout << "Move Assignment is called! source: " << str.m_data << std::endl;
        if (this != &str) {
            m_len = str.m_len;
            m_data = str.m_data; //避免了不必要的拷贝
            str.m_len = 0;
            str.m_data = nullptr;
        }
        return *this;
    }

    /**
     * 虚析构函数
     */
    virtual ~MyString() {
        if (m_data) free(m_data);
    }
};

int main() {
    // Default Constructor is called!
    MyString a;
    // Ordinary Constructor is called! source: Hello
    a = MyString("Hello");
    // Move Assignment is called! source: Hello
    MyString b = a;
    // Copy Constructor is called! source: Hello
    // Move Constructor is called! source: Hello
    MyString c = std::move(a);
    std::vector<MyString> vec;
    // Ordinary Constructor is called! source: World
    // Move Constructor is called! source: World
    vec.push_back(MyString("World"));
    return 0;
}
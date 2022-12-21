//
// Created by fsindustry on 2022/12/21.
//
#include <iostream>
#include <memory>
#include <cassert>


void demo_initial() {
    // create a shared_ptr pointed to a int value stored in heap.
    std::unique_ptr<int> p1(new int(1));
    p1.release(); // release p1
    assert(!p1);
    // std::unique_ptr<int> p2 = p1; // can't do this
    // another way the assign value to unique_ptr.
    std::unique_ptr<int> p3;
    p3.reset(new int(1));
    // more better and effecient way to inital unique_ptr
    auto p4 = std::make_unique<int>(100);

    // swap p3 and p4
    p3.swap(p4);
    assert((*p3) == 100 && (*p4) == 1);

    // move p3 to p5
    std::unique_ptr<int> p5 = std::move(p3);
    assert(!p3 && p5 && (*p5) == 100);
}

/**
 * custom deleter by function
 * @param p the ptr need to be cleaned
 */
void function_deleter(int *p) {
    std::cout << "deleting p from function..." << std::endl;
    delete p;
}

/**
 * custom deleter by lambda expression
 */
auto lambda_deleter = [](const int *p) {
    std::cout << "deleting p from lambda..." << std::endl;
    delete p;
};

/**
 * custom deleter by class
 */
class class_deleter {
public:
    void operator()(int *p) {
        std::cout << "deleting p from class..." << std::endl;
        delete p;
    }
};

void demo_deleter() {
    // create a unique_ptr with a function deleter
    std::unique_ptr<int, void (*)(int *)> p1(new int(1), function_deleter);
    // create a unique_ptr with a lambda expression deleter
    std::unique_ptr<int, decltype(lambda_deleter)> p2(new int(2), lambda_deleter);
    // create a unique_ptr with a class deleter
    class_deleter classDeleter;
    std::unique_ptr<int, class_deleter> p3(new int(3), classDeleter);
}

void demo_array() {
    // create a unique_ptr pointed to an array stored in heap.
    std::unique_ptr<int[]> p1(new int[10]);
    p1[5] = 5;
    // create a unique_ptr with a lambda expression deleter
    auto deleter = [](int *p) {
        std::cout << "deleting array pfrom lambda..." << std::endl;
        delete[] p;
    };
    std::unique_ptr<int[], decltype(deleter)> p2(new int[100], deleter);
    p2[10] = 10;
}


int main() {
    std::cout << "unique_ptr start..." << std::endl;
    demo_initial();
    demo_deleter();
    demo_array();
    std::cout << "... unique_ptr end" << std::endl;
}
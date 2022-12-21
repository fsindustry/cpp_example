//
// Created by fsindustry on 2022/12/21.
//
#include <iostream>
#include <memory>
#include <cassert>


void demo_initial() {
    // create a shared_ptr pointed to a int value stored in heap.
    std::shared_ptr<int> p1(new int(1));
    // add another ref to the int value.
    std::shared_ptr<int> p2 = p1;
    // another way the assign value to shared_ptr.
    std::shared_ptr<int> p3;
    p3.reset(new int(1));
    // more better and effecient way to inital shared_ptr
    auto p4 = std::make_shared<int>(100);

    assert(p2.get() == p1.get());
    assert(p2.use_count() == p1.use_count() && p1.use_count() == 2);
    assert(p3 && p3.unique());
    assert(p4 && p4.unique());
    p1.reset();
    assert(!p1 && p2 && p2.use_count() == 1);
    p2.reset();
    assert(!p1 && !p2);
}

/**
 * custom deleter by function
 * @param p the ptr need to be cleaned
 */
void function_deleter(const int *p) {
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
    void operator()(const int *p) {
        std::cout << "deleting p from class..." << std::endl;
        delete p;
    }
};

void demo_deleter() {
    // create a shared_ptr with a function deleter
    std::shared_ptr<int> p1(new int(1), function_deleter);
    // create a shared_ptr with a lambda expression deleter
    std::shared_ptr<int> p2(new int(2), lambda_deleter);
    // create a shared_ptr with a class deleter
    class_deleter classDeleter;
    std::shared_ptr<int> p3(new int(3), classDeleter);
}

class A : public std::enable_shared_from_this<A> {
public:
    std::shared_ptr<A> getSelf() {
        // can't return shared_ptr(this) directly,
        // because the shared_ptr is a different one.
        // return shared_ptr(this); // don't do this.
        return shared_from_this();
    }

    ~A() {
        std::cout << "deallocating..." << std::endl;
    }
};

void demo_shared_this() {
    std::shared_ptr<A> p1(new A);
    std::shared_ptr<A> p2 = p1->getSelf();

    assert(p1.get() == p2.get());
    assert(p1.use_count() == p2.use_count() && p1.use_count() == 2);
    p1.reset();
    assert(!p1 && p2 && p2.unique());
    p2.reset();
    assert(!p1 && !p2);
}


int main() {
    std::cout << "shared_ptr start..." << std::endl;
    demo_initial();
    demo_deleter();
    demo_shared_this();
    std::cout << "... shared_ptr end" << std::endl;
}
//
// Created by fsindustry on 2022/12/21.
//
#include <iostream>
#include <memory>
#include <cassert>


void demo_initial() {
    // create a shared_ptr pointed to a int value stored in heap.
    std::shared_ptr<int> p1(new int(1));
    // create weak_ptr with a shared_ptr
    std::weak_ptr<int> p2(p1);
    // create weak_ptr with a weak_ptr
    std::weak_ptr<int> p3(p2);
    // create weak_ptr with copy constructor
    std::weak_ptr<int> p4 = p1;

    assert(!p2.expired() && !p3.expired() && p2.use_count() == p1.use_count() && p2.use_count() == 1);
    assert(p2.lock() == p1);
    p1.reset();
    assert(p2.expired() && p3.expired() && !p4.lock());
}

void demo_scope() {
    std::weak_ptr<int> wp;
    std::shared_ptr<int> sp_ok;
    {
        std::shared_ptr<int> sp(new int(1));
        wp = sp;
        sp_ok = wp.lock(); // will increase the running_count
        assert(sp_ok.use_count() == 2);
    }
    if (wp.expired()) {
        std::cout << "shared_ptr is destroyed" << std::endl;
    } else {
        // will come here
        std::cout << "shared_ptr is not destroyed" << std::endl;
    }
}

int main() {
    std::cout << "unique_ptr start..." << std::endl;
    demo_initial();
    demo_scope();
    std::cout << "... unique_ptr end" << std::endl;
}
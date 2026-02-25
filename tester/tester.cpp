#include <iostream>
#include <cassert>
#include "../library/library.hpp"

void test_constructors() {
    std::cout << "Testing constructors..." << std::endl;

    // Default constructor
    Array a1;
    assert(a1.size() == 0);

    // Parameterized constructor
    Array a2(10);
    assert(a2.size() == 10);
    for (int i = 0; i < 10; ++i) {
        a2[i] = i * 2;
    }

    // Copy constructor
    Array a3(a2);
    assert(a3.size() == 10);
    for (int i = 0; i < 10; ++i) {
        assert(a3[i] == i * 2);
    }
    // Verify deep copy
    a3[0] = 100;
    assert(a2[0] == 0);

    // Move constructor
    Array a4(std::move(a3));
    assert(a4.size() == 10);
    assert(a4[0] == 100);
    assert(a3.size() == 0);

    std::cout << "Constructors test passed!" << std::endl;
}

void test_access() {
    std::cout << "Testing element access..." << std::endl;

    Array a(5);
    for (int i = 0; i < 5; ++i) {
        a.at(i) = i * 10;
    }

    // operator[] and at()
    for (int i = 0; i < 5; ++i) {
        assert(a[i] == i * 10);
        assert(a.at(i) == i * 10);
    }

    // Const access
    const Array& ca = a;
    assert(ca[2] == 20);
    assert(ca.at(3) == 30);

    std::cout << "Element access test passed!" << std::endl;
}

void test_assignment() {
    std::cout << "Testing assignment operators..." << std::endl;

    Array a1(5);
    for (int i = 0; i < 5; ++i) a1[i] = i;

    // Copy assignment
    Array a2;
    a2 = a1;
    assert(a2.size() == 5);
    for (int i = 0; i < 5; ++i) assert(a2[i] == i);
    
    // Verify deep copy
    a2[0] = 99;
    assert(a1[0] == 0);

    // Move assignment
    Array a3;
    a3 = std::move(a2);
    assert(a3.size() == 5);
    assert(a3[0] == 99);
    assert(a2.size() == 0);

    // Self assignment check (implementation should handle this)
    a3 = a3;
    assert(a3.size() == 5);
    assert(a3[0] == 99);

    std::cout << "Assignment operators test passed!" << std::endl;
}

void test_memory_management() {
    std::cout << "Testing memory management..." << std::endl;

    Array a(10);
    assert(a.size() == 10);

    a.allocate(20);
    assert(a.size() == 20);
    
    a.clear();
    assert(a.size() == 0);

    a.allocate(5);
    assert(a.size() == 5);

    std::cout << "Memory management test passed!" << std::endl;
}

int main() {
    try {
        test_constructors();
        test_access();
        test_assignment();
        test_memory_management();
        std::cout << "\nAll tests passed successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown error." << std::endl;
        return 1;
    }
    return 0;
}

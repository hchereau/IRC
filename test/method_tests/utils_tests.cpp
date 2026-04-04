#include <iostream>
#include "tests.hpp"

void printTestResult(const std::string& testName, bool condition) {
    if (condition) {
        std::cout << "[\033[32mOK\033[0m] " << testName << std::endl;
    } else {
        std::cerr << "[\033[31mKO\033[0m] " << testName << std::endl;
    }
    assert(condition);
}
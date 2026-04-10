#include "Client.hpp"
#include "Channel.hpp"
#include "tests.hpp"
#include <iostream>

int main() {
    std::cout << "=== FT_IRC GLOBAL TEST SUITE ===\n" << std::endl;

    runClientTests();
    runChannelTests();
    runValidationTest();
    test_out_of_memory();
    testDispatcherOnly();
    runParsingTests();

    std::cout << "\n=== All tests completed ===" << std::endl;
    return 0;
}
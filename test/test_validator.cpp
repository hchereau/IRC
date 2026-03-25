#include "tests.hpp"
#include <iostream>
#include <vector>
#include "CommandValidator.hpp"


namespace {

void runChannelValidationTests() { 
    std::cout << "\n--- TESTS CHANNEL VALIDATION ---" << std::endl;
    printTestResult("Valid Channel", CommandValidator::isValidChannelName("#general") == true);
    printTestResult("No #", CommandValidator::isValidChannelName("general") == false);
    printTestResult("Forbidden space", CommandValidator::isValidChannelName("#gen eral") == false);
    printTestResult("Empty", CommandValidator::isValidChannelName("") == false);
}

void runNicknameValidationTests() {
    std::cout << "\n--- TESTS NICKNAME VALIDATION ---" << std::endl;
    printTestResult("Valid Nick", CommandValidator::isValidNickname("Robert") == true);
    printTestResult("Forbidden start (#)", CommandValidator::isValidNickname("#Rob") == false);
    printTestResult("Forbidden space", CommandValidator::isValidNickname("Rob ert") == false);
    printTestResult("Empty", CommandValidator::isValidNickname("") == false);
}

void runParamsValidationTests() {
    std::cout << "\n--- TESTS MIN PARAMS VALIDATION ---" << std::endl;
    std::vector<std::string> params;
    
    printTestResult("Empty Vector (needs 1)", CommandValidator::hasMinParams(params, 1) == false);
    printTestResult("Empty Vector (needs 0)", CommandValidator::hasMinParams(params, 0) == true);

    params.push_back("#general");
    printTestResult("1 Param (needs 1)", CommandValidator::hasMinParams(params, 1) == true);
    printTestResult("1 Param (needs 2)", CommandValidator::hasMinParams(params, 2) == false);
}

void runTrailingValidationTests() {
    std::cout << "\n--- TESTS TRAILING VALIDATION ---" << std::endl;
    printTestResult("Empty Trailing", CommandValidator::isTrailingParamValid("") == false);
    printTestResult("Valid Trailing", CommandValidator::isTrailingParamValid("Salut les gars") == true);
}

}

void    runValidationTest() {
    runChannelValidationTests();
    runNicknameValidationTests();
    runParamsValidationTests();
    runTrailingValidationTests();

}
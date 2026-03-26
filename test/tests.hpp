#pragma once
#include <string>
#include <assert.h>

void    runClientTests();
void    runChannelTests();
void    printTestResult(const std::string& testName, bool condition);
void    runValidationTest();

#pragma once
#include <string>

// On déclare les "Suites de tests"
void runClientTests();
void runChannelTests();
void printTestResult(const std::string& testName, bool condition);

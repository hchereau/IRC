#pragma once

#include <string>
#include <vector>

class CommandValidator {
private:
    CommandValidator();
    ~CommandValidator();

public:
    static bool hasMinParams(const std::vector<std::string>& params, size_t min);

    static bool isValidChannelName(const std::string& name);
    static bool isValidNickname(const std::string& nick);
    
    static bool isTrailingParamValid(const std::string& trailing);
};
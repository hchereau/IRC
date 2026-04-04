#include "CommandValidator.hpp"

bool CommandValidator::hasMinParams(const std::vector<std::string>& params, size_t min) {
    return (params.size() >= min);
}

bool CommandValidator::isValidChannelName(const std::string& name) {
    if (name.empty()) {
        return false;
    }
    if (name[0] != '#' && name[0] != '&') {
        return false;
    }
    for (size_t i = 0; i < name.length(); ++i) {
        if (name[i] == ' ' || name[i] == ',' || name[i] == 7) {
            return false;
        }
    }
    return true;
}

bool CommandValidator::isValidNickname(const std::string& nick) {
    if (nick.empty() || nick.length() > 9)
        return false;
    
    if (!isalpha(nick[0])) 
        return false;

    for (size_t i = 1; i < nick.length(); ++i) {
        if (!isalnum(nick[i]) && nick[i] != '_' && nick[i] != '-') {
            return false;
        }
    }
    return true;
}

bool CommandValidator::isTrailingParamValid(const std::string& trailing) {
    return (!trailing.empty());
}


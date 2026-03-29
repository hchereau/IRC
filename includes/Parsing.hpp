#pragma once
#include <string>
#include <vector>

struct Message {
    std::string prefix; // not always, if ":" present
    std::string cmd;
    std::vector<std::string> params;
    std::string trailing; // message, starts with ":"
};
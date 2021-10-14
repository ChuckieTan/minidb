#pragma once
#include <iostream>
#include <string>
#include <vector>

namespace minidb::util {
class Input {
public:
    Input();
    std::string buffer;
    bool        readInputFromStream(std::istream &stream);
};
} // namespace minidb

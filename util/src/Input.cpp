#include "Input.h"
#include "MinidbStrings.h"
#include <iostream>
#include <string>


namespace minidb::util {
Input::Input() {
    buffer = std::string();
}

bool Input::readInputFromStream(std::istream &stream) {
    if(buffer.size() != 0) {
        buffer = "";
    }
    MinidbStrings::print_string(MinidbStrings::StringType::full_pragma);
    std::string line;
    std::getline(stream, line);
    buffer.append(line);
    while (line.back() != ';') {
        if (typeid(stream) == typeid(std::cin)) {
            MinidbStrings::print_string(
                MinidbStrings::StringType::light_pragma);
        }
        getline(stream, line);
        buffer.append(line);
    }
    return true;
}
} // namespace minidb
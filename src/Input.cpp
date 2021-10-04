#include "Input.h"
#include "Strings.h"
#include <iostream>
#include <string>


namespace minidb {
Input::Input() {
    buffer = std::string();
}

bool Input::readInputFromStream(std::istream &stream) {
    Strings::print_string(Strings::StringType::full_pragma);
    std::string line;
    std::getline(stream, line);
    buffer.append(line);
    while (line.back() != ';') {
        if (typeid(stream) == typeid(std::cin)) {
            Strings::print_string(Strings::StringType::light_pragma);
        }
        getline(stream, line);
        buffer.append(line);
    }
    return true;
}
} // namespace minidb
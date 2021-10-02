#include "Strings.h"
#include <iostream>
#include <string>
#include <unordered_map>

namespace minidb {

std::unordered_map<Strings::StringType, std::string> Strings::strings{
    {Strings::StringType::info, "Minidb version 0.0.1\n"},
    {Strings::StringType::full_pragma, "minidb> "},
    {Strings::StringType::light_pragma, "    >>> "}};

void Strings::print_string(StringType stringType) {
    std::cout << strings[ stringType ];
}
} // namespace minidb
#include "MinidbStrings.h"
#include <iostream>
#include <string>
#include <unordered_map>
#include <spdlog/spdlog.h>

namespace minidb::util {

std::unordered_map<MinidbStrings::StringType, std::string>
    MinidbStrings::strings{
        { MinidbStrings::StringType::info, "Minidb version 0.0.1\n" },
        { MinidbStrings::StringType::full_pragma, "minidb> " },
        { MinidbStrings::StringType::light_pragma, "    >>> " }
    };

void MinidbStrings::print_string(StringType stringType) {
    fmt::print(strings[ stringType ]);
}
} // namespace minidb
#pragma once
#include <string>
#include <unordered_map>
#include <vector>

namespace minidb {
class MinidbStrings {
public:
    enum class StringType { info, full_pragma, light_pragma };
    static void print_string(StringType stringType);

private:
    MinidbStrings() = default;
    static std::unordered_map<StringType, std::string> strings;
};
} // namespace minidb
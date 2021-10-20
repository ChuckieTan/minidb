#pragma once

#include "SQLBinaryData.h"
#include "SQLExprValue.h"
#include <vector>

namespace minidb::operate {

class BinaryOperate {
public:
    static storage::SQLBinaryData
        dump(const std::vector<ast::SQLExprValue> &values);

    static std::vector<ast::SQLExprValue> load(storage::SQLBinaryData);

    static const std::int8_t DATA_INT    = 0;
    static const std::int8_t DATA_FLOAT  = 1;
    static const std::int8_t DATA_STRING = 2;
};
} // namespace minidb::operate
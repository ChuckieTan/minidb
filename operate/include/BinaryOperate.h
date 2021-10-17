#pragma once

#include "SQLBinaryData.h"
#include "SQLExprValue.h"
#include <vector>

namespace minidb::operate {

class BiaryOperate {
public:
    static SQLBinaryData dumps(const std::vector<ast::SQLExprValue> &values);
    static std::vector<ast::SQLExprValue> loads(SQLBinaryData);
};
} // namespace minidb::operate
#pragma once

#include "SQLExprValue.h"
#include <string>

namespace minidb::ast {
class SQLExpr {
public:
    SQLExprValue lValue;
    SQLExprValue rValue;
    std::string op;
};

} // namespace minidb::ast
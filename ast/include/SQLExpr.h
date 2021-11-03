#pragma once

#include "SQLExprValue.h"
#include "TokenType.h"
#include <string>

namespace minidb::ast {
class SQLExpr {
public:
    SQLExprValue lValue;
    SQLExprValue rValue;
    parser::TokenType op;
};

} // namespace minidb::ast
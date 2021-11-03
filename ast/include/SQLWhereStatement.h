#pragma once

#include "SQLExpr.h"
namespace minidb::ast {

class SQLWhereStatement {
public:
    SQLExpr expr;
    bool isEmpty;
};
} // namespace minidb::ast
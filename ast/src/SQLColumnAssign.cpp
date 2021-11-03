#include "SQLColumnAssign.h"

namespace minidb::ast {

bool SQLColumnAssign::operator==(const SQLColumnAssign &assign) const {
    return columnName == assign.columnName && value == assign.value;
}
} // namespace minidb::ast
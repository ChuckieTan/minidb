#pragma once

#include "Pager.h"
#include <cstdint>
#include <string>
#include <vector>

namespace minidb::storage {

class Storage {
public:
    struct TablePos {
        std::string  tableName;
        std::int32_t pos;

        TablePos(const std::string &_tableName, std::int32_t _pos);
    };

    Storage(const std::string &_fileName, bool _isInMemory);

    std::vector<TablePos> tablePosList;
    Pager                 pager;

    std::uint32_t getTablePos();
    std::uint32_t createNewTable();
    std::uint32_t newPageBlock();

    std::uint32_t              tableNum;
    static const std::uint32_t pageSize         = 4096;
    static const std::uint32_t tableNumAddr     = 7;
    static const std::uint32_t tableDefineBegin = 11;
    std::uint32_t              tableDefineEnd   = 0;
};
} // namespace minidb::storage
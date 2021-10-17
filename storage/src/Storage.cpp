#include "Storage.h"
#include "Pager.h"
#include "spdlog/spdlog.h"
#include <cstdint>
#include <cstring>
#include <string>

namespace minidb::storage {

static const char *tag = "Minidb";

Storage::TablePos::TablePos(const std::string &_tableName, std::int32_t _pos)
    : tableName(_tableName)
    , pos(_pos) {}

Storage::Storage(const std::string &_fileName, bool _isInMemory)
    : pager(_fileName, _isInMemory) {
    if (pager.getFileSize() == 0) {
        // 写入 4KB 作为 Metadata
        char *head = new char[ pageSize ];
        pager.write(head, pageSize, 0);
        delete[] head;

        // 写入 Minidb 标识
        pager.write(tag, std::strlen(tag), 0);

        tableNum       = 0;
        tableDefineEnd = tableDefineBegin;

        // 写入 tableNum
        pager.write((char *) &tableNum, sizeof(tableNum), tableNumAddr);
    } else {
        // 判断文件开头是否有 Minidb 标识
        char head[ 7 ];
        pager.read(0, head, sizeof(head));
        if (std::strcmp(head, tag) != 0) {
            spdlog::error("{} is not a Minidb database file", _fileName);
        }

        // 获取表的数量
        pager.read(tableNumAddr, (char *) &tableNum, sizeof(tableNum));

        // 循环获取所有表的表名和地址
        std::uint32_t currentAddr = tableDefineBegin;
        for (std::uint32_t i = 0; i < tableNum; i++) {
            // 获取当前表的大小
            std::uint32_t tableNameSize;
            pager.read(currentAddr, (char *) &tableNameSize,
                       sizeof(tableNameSize));

            currentAddr += sizeof(tableNameSize);

            // 获取当前表名
            std::vector<char> tableNameSeq(tableNameSize);
            pager.read(currentAddr, tableNameSeq.data(), tableNameSize);
            std::string tableName;
            tableName.insert(tableName.begin(), tableNameSeq.begin(),
                             tableNameSeq.end());

            currentAddr += tableNameSize;

            // 获取表的首个节点地址
            std::uint32_t tableAddr;
            pager.read(currentAddr, (char *) &tableAddr, sizeof(tableAddr));

            tablePosList.push_back(TablePos(tableName, tableAddr));
            spdlog::info("scan table {}", tableName);
        }
    }
}


} // namespace minidb::storage
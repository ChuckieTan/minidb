#pragma once

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>

#include "SQLBinaryData.h"

namespace minidb::storage {

class Pager {
public:
    Pager(const std::string &_fileName, bool _isInMemory);
    ~Pager();

    std::uint32_t writeRow(SQLBinaryData data, std::uint32_t pos = 0);

    std::uint32_t write(SQLBinaryData data, std::uint32_t pos);

    std::uint32_t write_back(SQLBinaryData data);

    bool                   read(SQLBinaryData data,std::uint32_t pos );
    storage::SQLBinaryData readRow(std::uint32_t pos);

    std::uint32_t getFileSize();
    bool          isInMemory;
    std::fstream  dataFile;
};
} // namespace minidb::storage
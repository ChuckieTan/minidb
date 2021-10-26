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

    std::uint32_t writeRow(void *data, std::uint32_t size,
                           std::uint32_t pos = 0);

    std::uint32_t write(const void *data, std::uint32_t size,
                        std::uint32_t pos);

    std::uint32_t write_back(const void *data, std::uint32_t size);

    bool read(void *data, std::uint32_t size, std::uint32_t pos);
    storage::SQLBinaryData readRow(std::uint32_t pos);

    std::uint32_t getFileSize();
    bool          isInMemory;
    std::fstream  dataFile;
};
} // namespace minidb::storage
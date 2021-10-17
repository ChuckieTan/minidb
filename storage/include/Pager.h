#pragma once

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>

namespace minidb::storage {

class Pager {
public:
    Pager(const std::string &_fileName, bool _isInMemory);
    ~Pager();

    std::uint32_t writeRow(const char *data, std::uint32_t size,
                           std::uint32_t pos = 0);

    std::uint32_t write(const char *data, std::uint32_t size,
                        std::uint32_t pos);

    std::uint32_t write_back(const char *data, std::uint32_t size);
    std::uint32_t getRowSize(std::uint32_t pos);
    bool          read(std::uint32_t pos, char *data, std::uint32_t size);
    bool          readRow(std::uint32_t pos, char *data);

    std::uint32_t getFileSize();
    bool          isInMemory;
    std::fstream  dataFile;
};
} // namespace minidb::storage
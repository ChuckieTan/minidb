#pragma once

#include <c++/10/bits/c++config.h>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace minidb::storage {

class Pager {
public:
    Pager(const std::string &_fileName, bool _isInMemory);

    int write(const char *data, std::int32_t size);

    std::int32_t     getSize(std::int32_t pos);
    bool             read(std::int32_t pos, char *data, std::int32_t size);

    // char *        readBuffer;
    // char *        writeBuffer;
    // std::uint32_t readBufferPos;

    bool                isInMemory;
    // std::vector<char *> inMemoryData;
    std::fstream        dataFile;
};
} // namespace minidb::storage
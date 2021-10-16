#include "Pager.h"
#include "spdlog/spdlog.h"
#include <c++/10/bits/c++config.h>
#include <fstream>
#include <string>

namespace minidb::storage {

Pager::Pager(const std::string &_fileName, bool _isInMemory = true)
    : isInMemory(_isInMemory) {
    if (!_isInMemory) {
        dataFile =
            std::fstream(_fileName, std::fstream::in | std::fstream::app |
                                        std::fstream::binary);
    }
    // readBuffer = new char[ 4096 ];
    // writeBuffer = new char[ 4096 ];
}

int Pager::write(const char *data, std::int32_t size) {
    dataFile.seekp(0, dataFile.end);
    int addr = dataFile.tellp();
    dataFile.write((char *)&size, sizeof(size));
    dataFile.write(data, size);
    return addr;
}

std::int32_t Pager::getSize(std::int32_t pos) {
    std::int32_t size;
    dataFile.seekg(pos, dataFile.beg);
    dataFile.read((char *) &size, sizeof(size));
    return size;
}

bool Pager::read(std::int32_t pos, char *data, std::int32_t size) {
    dataFile.seekg(0, dataFile.end);
    if (pos <= dataFile.tellg()) {
        dataFile.seekg(pos, dataFile.beg);
        dataFile.read(data, size);
        return true;
    } else {
        spdlog::error("read file out of file size");
        return false;
    }
}
} // namespace minidb::storage
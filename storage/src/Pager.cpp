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
}

Pager::~Pager() { dataFile.close(); }

std::uint32_t Pager::writeRow(const char *data, std::uint32_t size,
                              std::uint32_t pos) {
    std::uint32_t addr = 0;
    if (pos == 0) {
        dataFile.seekp(0, dataFile.end);
        addr = dataFile.tellp();
        dataFile.write((char *) &size, sizeof(size));
        dataFile.write(data, size);
    } else {
        dataFile.seekp(pos, dataFile.beg);
        addr = dataFile.tellp();
        dataFile.write((char *) &size, sizeof(size));
        dataFile.write(data, size);
    }
    return addr;
}

std::uint32_t Pager::write(const char *data, std::uint32_t size,
                           std::uint32_t pos) {
    std::uint32_t addr = 0;
    dataFile.seekp(pos, dataFile.beg);
    addr = dataFile.tellp();
    dataFile.write(data, size);
    return addr;
}

std::uint32_t Pager::write_back(const char *data, std::uint32_t size) {
    std::uint32_t addr = 0;
    dataFile.seekp(0, dataFile.end);
    addr = dataFile.tellp();
    dataFile.write(data, size);
    return addr;
}

std::uint32_t Pager::getRowSize(std::uint32_t pos) {
    std::uint32_t size;
    dataFile.seekg(pos, dataFile.beg);
    dataFile.read((char *) &size, sizeof(size));
    return size;
}

bool Pager::read(std::uint32_t pos, char *data, std::uint32_t size) {
    if (pos <= getFileSize()) {
        dataFile.seekg(pos, dataFile.beg);
        dataFile.read(data, size);
        return true;
    } else {
        spdlog::error("read file out of file size");
        return false;
    }
}

bool Pager::readRow(std::uint32_t pos, char *data) {
    if (pos <= getFileSize()) {
        std::uint32_t size;
        dataFile.seekg(pos, dataFile.beg);
        dataFile.read((char*)&size, sizeof(size));
        dataFile.read(data, size);
        return true;
    } else {
        spdlog::error("read file out of file size");
        return false;
    }
}

std::uint32_t Pager::getFileSize() {
    // auto mark = dataFile.tellg();
    dataFile.seekg(0, dataFile.end);
    auto ans = dataFile.tellg();
    // dataFile.seekg(mark, dataFile.beg);
    return ans;
}
} // namespace minidb::storage
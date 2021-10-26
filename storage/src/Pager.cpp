#include "Pager.h"
#include "SQLBinaryData.h"
#include "spdlog/spdlog.h"
#include <fstream>
#include <memory>
#include <ostream>
#include <string>

namespace minidb::storage {

Pager::Pager(const std::string &_fileName, bool _isInMemory = true)
    : isInMemory(_isInMemory) {
    if (!_isInMemory) {
        dataFile =
            std::fstream(_fileName, std::fstream::out | std::fstream::in |
                                        std::fstream::binary);
        if (dataFile.fail()) {
            std::ofstream fout(_fileName);
            fout.close();
            dataFile.clear();
            dataFile.close();
            dataFile =
                std::fstream(_fileName, std::fstream::out | std::fstream::in |
                                            std::fstream::binary);
        }
    }
}

Pager::~Pager() { dataFile.close(); }

std::uint32_t Pager::writeRow(void *data, std::uint32_t size,
                              std::uint32_t pos) {
    std::uint32_t addr = 0;
    if (pos == 0) {
        dataFile.seekp(0, dataFile.end);
        addr = dataFile.tellp();
        dataFile.write((char *) &size, sizeof(size));
        dataFile.write((char *) data, size);
    } else {
        dataFile.seekp(pos, dataFile.beg);
        addr = dataFile.tellp();
        dataFile.write((char *) &size, sizeof(size));
        dataFile.write((char *) data, size);
    }
    return addr;
}

std::uint32_t Pager::write(const void *data, std::uint32_t size,
                           std::uint32_t pos) {
    std::uint32_t addr = 0;
    dataFile.seekp(pos, dataFile.beg);
    addr = dataFile.tellp();
    dataFile.write((char *) data, size);
    return addr;
}

std::uint32_t Pager::write_back(const void *data, std::uint32_t size) {
    std::uint32_t addr = 0;
    dataFile.seekp(0, dataFile.end);
    addr = dataFile.tellp();
    dataFile.write((char *) data, size);
    return addr;
}

bool Pager::read(void *data, std::uint32_t size, std::uint32_t pos) {
    if (pos <= getFileSize()) {
        dataFile.seekg(pos, dataFile.beg);
        dataFile.read((char *) data, size);
        return true;
    } else {
        spdlog::error("read file out of file size");
        return false;
    }
}

storage::SQLBinaryData Pager::readRow(std::uint32_t pos) {
    if (pos <= getFileSize()) {
        std::uint32_t size;
        dataFile.seekg(pos, dataFile.beg);
        dataFile.read((char *) &size, sizeof(size));
        SQLBinaryData data(size);

        auto addr = data.data.get();
        dataFile.read(addr, size);
        return data;
    } else {
        spdlog::error("read file out of file size");
        return SQLBinaryData(0);
    }
}

std::uint32_t Pager::getFileSize() {
    dataFile.seekg(0, dataFile.beg);
    std::uint32_t beg = dataFile.tellg();

    dataFile.seekg(0, dataFile.end);
    std::uint32_t ed = dataFile.tellg();
    return ed - beg;
}
} // namespace minidb::storage
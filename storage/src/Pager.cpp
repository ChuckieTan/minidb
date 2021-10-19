#include "Pager.h"
#include "SQLBinaryData.h"
#include "spdlog/spdlog.h"
#include <c++/10/bits/c++config.h>
#include <fstream>
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

std::uint32_t Pager::writeRow(SQLBinaryData data, std::uint32_t pos) {
    std::uint32_t addr = 0;
    if (pos == 0) {
        dataFile.seekp(0, dataFile.end);
        addr = dataFile.tellp();
        dataFile.write((char *) &data.size, sizeof(data.size));
        dataFile.write(data.data, data.size);
    } else {
        dataFile.seekp(pos, dataFile.beg);
        addr = dataFile.tellp();
        dataFile.write((char *) &data.size, sizeof(data.size));
        dataFile.write(data.data, data.size);
    }
    return addr;
}

std::uint32_t Pager::write(SQLBinaryData data, std::uint32_t pos) {
    std::uint32_t addr = 0;
    dataFile.seekp(pos, dataFile.beg);
    addr = dataFile.tellp();
    dataFile.write(data.data, data.size);
    return addr;
}

std::uint32_t Pager::write_back(SQLBinaryData data) {
    std::uint32_t addr = 0;
    dataFile.seekp(0, dataFile.end);
    addr = dataFile.tellp();
    dataFile.write(data.data, data.size);
    return addr;
}

bool Pager::read(SQLBinaryData data, std::uint32_t pos) {
    if (pos <= getFileSize()) {
        dataFile.seekg(pos, dataFile.beg);
        dataFile.read(data.data, data.size);
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
        auto data = new char[ size ];
        dataFile.read(data, size);
        return { data, size };
    } else {
        spdlog::error("read file out of file size");
        return { nullptr, 0 };
    }
}

std::uint32_t Pager::getFileSize() {
    // auto mark = dataFile.tellg();
    dataFile.seekg(0, dataFile.beg);
    int beg = dataFile.tellg();

    dataFile.seekg(0, dataFile.end);
    int ed = dataFile.tellg();
    // dataFile.seekg(mark, dataFile.beg);
    return ed - beg;
}
} // namespace minidb::storage
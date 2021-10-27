#include "Pager.h"
#include "SQLBinaryData.h"
#include "spdlog/spdlog.h"
#include <fstream>
#include <memory>
#include <ostream>
#include <string>

namespace minidb::storage {

Pager::Pager(const std::string &_file_name, bool _isInMemory = true)
    : is_in_memory(_isInMemory) {
    std::string index_file_name, data_file_name;
    if (_file_name.back() == 'b') {
        data_file_name = _file_name;
        index_file_name =
            _file_name.substr(0, _file_name.rfind('.')) + ".index";
    } else {
        index_file_name = _file_name;
        data_file_name  = _file_name.substr(0, _file_name.rfind('.')) + ".db";
    }

    if (!_isInMemory) {
        index_file =
            std::fstream(index_file_name, std::fstream::out | std::fstream::in |
                                              std::fstream::binary);
        if (index_file.fail()) {
            std::ofstream fout(index_file_name);
            fout.close();
            index_file.clear();
            index_file.close();
            index_file = std::fstream(index_file_name,
                                      std::fstream::out | std::fstream::in |
                                          std::fstream::binary);
        }

        data_file =
            std::fstream(data_file_name, std::fstream::out | std::fstream::in |
                                             std::fstream::binary);
        if (data_file.fail()) {
            std::ofstream fout(data_file_name);
            fout.close();
            data_file.clear();
            data_file.close();
            data_file = std::fstream(data_file_name, std::fstream::out |
                                                         std::fstream::in |
                                                         std::fstream::binary);
        }
    }
}

Pager::~Pager() { index_file.close(); }

// 往数据文件里写入一行数据，并返回首地址
std::uint32_t Pager::write_row(void *data, std::uint32_t size,
                               std::uint32_t pos) {
    std::uint32_t addr = 0;
    if (pos == 0) {
        data_file.seekp(0, data_file.end);
        addr = data_file.tellp();
        data_file.write((char *) &size, sizeof(size));
        data_file.write((char *) data, size);
    } else {
        data_file.seekp(pos, data_file.beg);
        addr = data_file.tellp();
        data_file.write((char *) &size, sizeof(size));
        data_file.write((char *) data, size);
    }
    return addr;
}

std::uint32_t Pager::write_index_file(const void *data, std::uint32_t size,
                                      std::uint32_t pos) {
    std::uint32_t addr = 0;
    index_file.seekp(pos, index_file.beg);
    addr = index_file.tellp();
    index_file.write((char *) data, size);
    return addr;
}

std::uint32_t Pager::write_back(const void *data, std::uint32_t size) {
    std::uint32_t addr = 0;
    index_file.seekp(0, index_file.end);
    addr = index_file.tellp();
    index_file.write((char *) data, size);
    return addr;
}

bool Pager::read_index_file(void *data, std::uint32_t size, std::uint32_t pos) {
    if (pos <= get_index_file_size()) {
        index_file.seekg(pos, index_file.beg);
        index_file.read((char *) data, size);
        return true;
    } else {
        spdlog::error("read index file out of file size");
        return false;
    }
}

std::uint32_t Pager::write_data_file(const void *data, std::uint32_t size,
                                     std::uint32_t pos) {
    std::uint32_t addr = 0;
    data_file.seekp(pos, index_file.beg);
    addr = index_file.tellp();
    data_file.write((char *) data, size);
    return addr;
}

std::uint32_t Pager::read_data_file(void *data, std::uint32_t size, std::uint32_t pos) {
    if (pos <= get_index_file_size()) {
        data_file.seekg(pos, index_file.beg);
        data_file.read((char *) data, size);
        return true;
    } else {
        spdlog::error("read data file out of file size");
        return false;
    }
}

// 从数据文件里读入一行数据，并返回
storage::SQLBinaryData Pager::read_row(std::uint32_t pos) {
    if (pos <= get_index_file_size()) {
        std::uint32_t size;
        data_file.seekg(pos, data_file.beg);
        data_file.read((char *) &size, sizeof(size));
        SQLBinaryData data(size);

        auto addr = data.data.get();
        data_file.read(addr, size);
        return data;
    } else {
        spdlog::error("read file out of file size");
        return SQLBinaryData(0);
    }
}

std::uint32_t Pager::get_index_file_size() {
    index_file.seekg(0, index_file.beg);

    std::uint32_t beg = index_file.tellg();

    index_file.seekg(0, index_file.end);
    std::uint32_t ed = index_file.tellg();
    return ed - beg;
}
} // namespace minidb::storage
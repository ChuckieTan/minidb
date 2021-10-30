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
    std::string index_file_name, data_file_name, journal_file_name;
    if (_file_name.back() == 'b') {
        data_file_name = _file_name;
        index_file_name =
            _file_name.substr(0, _file_name.rfind('.')) + ".index";
    } else {
        index_file_name = _file_name;
        data_file_name  = _file_name.substr(0, _file_name.rfind('.')) + ".db";
    }

    // 设置日志文件文件名
    journal_file_name =
        _file_name.substr(0, _file_name.rfind('.')) + ".journal";

    if (!_isInMemory) {
        // 打开 index 文件
        index_file =
            std::fstream(index_file_name, std::fstream::out | std::fstream::in |
                                              std::fstream::binary);
        // 如果文件不存在则创建文件
        if (index_file.fail()) {
            std::ofstream fout(index_file_name);
            fout.close();
            index_file.clear();
            index_file.close();
            index_file = std::fstream(index_file_name,
                                      std::fstream::out | std::fstream::in |
                                          std::fstream::binary);
        }

        // 打开数据文件
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

        // 打开日志文件
        journal_file = std::fstream(journal_file_name,
                                    std::fstream::out | std::fstream::in |
                                        std::fstream::binary);
        if (journal_file.fail()) {
            std::ofstream fout(journal_file_name);
            fout.close();
            journal_file.clear();
            journal_file.close();
            journal_file = std::fstream(journal_file_name,
                                        std::fstream::out | std::fstream::in |
                                            std::fstream::binary);
        }
    }
}

Pager::~Pager() {
    index_file.close();
    data_file.close();
    journal_file.close();
}

// 往数据文件里写入一行数据，并返回首地址
std::uint64_t Pager::write_row(void *data, std::uint64_t size,
                               std::uint64_t pos) {
    std::uint64_t addr = 0;
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

std::uint64_t Pager::write_index_file(const void *data, std::uint64_t size,
                                      std::uint64_t pos) {
    std::uint64_t addr = 0;
    index_file.seekp(pos, index_file.beg);
    addr = index_file.tellp();
    index_file.write((char *) data, size);
    return addr;
}

std::uint64_t Pager::new_page() {
    std::uint64_t addr = 0;
    index_file.seekp(0, index_file.end);
    addr      = index_file.tellp();
    auto data = std::make_unique<char[]>(4096);
    index_file.write(data.get(), 4096);
    return addr;
}

bool Pager::read_index_file(void *data, std::uint64_t size, std::uint64_t pos) {
    if (pos <= get_index_file_size()) {
        index_file.seekg(pos, index_file.beg);
        index_file.read((char *) data, size);
        return true;
    } else {
        spdlog::error("read index file out of file size");
        return false;
    }
}

std::uint64_t Pager::write_data_file(const void *data, std::uint64_t size,
                                     std::uint64_t pos) {
    std::uint64_t addr = 0;
    data_file.seekp(pos, data_file.beg);
    addr = data_file.tellp();
    data_file.write((char *) data, size);
    return addr;
}

std::uint64_t Pager::read_data_file(void *data, std::uint64_t size,
                                    std::uint64_t pos) {
    if (pos <= get_data_file_size()) {
        data_file.seekg(pos, data_file.beg);
        data_file.read((char *) data, size);
        return true;
    } else {
        spdlog::error("read data file out of file size");
        return false;
    }
}

// 从数据文件里读入一行数据，并返回
storage::SQLBinaryData Pager::read_row(std::uint64_t pos) {
    if (pos <= get_index_file_size()) {
        std::uint64_t size;
        data_file.seekg(pos, data_file.beg);
        data_file.read((char *) &size, sizeof(size));
        SQLBinaryData data(size);
 
        auto addr = data.data.get();
        data_file.read(addr, size);
        return data;
    } else {
        spdlog::error("read data file out of file size");
        return SQLBinaryData(0);
    }
}

std::uint64_t Pager::write_journal_file(const void *data, std::uint64_t size,
                                     std::uint64_t pos) {
    std::uint64_t addr = 0;
    journal_file.seekp(pos, journal_file.beg);
    addr = journal_file.tellp();
    journal_file.write((char *) data, size);
    return addr;
}

std::uint64_t Pager::read_journal_file(void *data, std::uint64_t size,
                                       std::uint64_t pos) {
    if (pos <= get_journal_file_size()) {
        journal_file.seekg(pos, journal_file.beg);
        journal_file.read((char *) data, size);
        return true;
    } else {
        spdlog::error("read journal file out of file size");
        return false;
    }
}

std::uint64_t Pager::get_index_file_size() {
    index_file.seekg(0, index_file.beg);

    std::uint64_t beg = index_file.tellg();

    index_file.seekg(0, index_file.end);
    std::uint64_t ed = index_file.tellg();
    return ed - beg;
}

std::uint64_t Pager::get_data_file_size() {
    data_file.seekg(0, data_file.beg);

    std::uint64_t beg = data_file.tellg();

    data_file.seekg(0, data_file.end);
    std::uint64_t ed = data_file.tellg();
    return ed - beg;
}

std::uint64_t Pager::get_journal_file_size() {
    journal_file.seekg(0, journal_file.beg);

    std::uint64_t beg = journal_file.tellg();

    journal_file.seekg(0, journal_file.end);
    std::uint64_t ed = journal_file.tellg();
    return ed - beg;
}
} // namespace minidb::storage
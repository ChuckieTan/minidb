#pragma once

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>

#include "SQLBinaryData.h"

namespace minidb::storage {

class Pager {
public:
    Pager(const std::string &_file_name, bool _is_in_memory);
    ~Pager();

    std::uint64_t write_row(void *data, std::uint64_t size,
                            std::uint64_t pos = 0);

    std::uint64_t write_index_file(const void *data, std::uint64_t size,
                                   std::uint64_t pos);

    std::uint64_t write_back(const void *data, std::uint64_t size);

    std::uint64_t write_data_file(const void *data, std::uint64_t size,
                                  std::uint64_t pos);
    std::uint64_t read_data_file(void *data, std::uint64_t size,
                                 std::uint64_t pos);
    bool read_index_file(void *data, std::uint64_t size, std::uint64_t pos);
    storage::SQLBinaryData read_row(std::uint64_t pos);

    std::uint64_t get_index_file_size();
    bool          is_in_memory;
    std::fstream  index_file;
    std::fstream  data_file;
};
} // namespace minidb::storage
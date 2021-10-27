#include <cstring>
#include <vector>

#include "BinaryOperate.h"
#include "SQLBinaryData.h"
#include "SQLExprValue.h"

namespace minidb::operate {

storage::SQLBinaryData
    BinaryOperate::dump(const std::vector<ast::SQLExprValue> &values) {
    std::uint64_t size = 0;

    // 计算二进制数据所需的大小
    for (const auto &value : values) {
        // 数据类型标识
        size += sizeof(std::uint8_t);

        if (value.isInt()) {
            size += sizeof(std::uint64_t);
        } else if (value.isFloat()) {
            size += sizeof(double);
        } else if (value.isString()) {
            // 字符串长度
            size += sizeof(std::uint64_t);
            size += value.getStringValue().size();
        }
    }
    storage::SQLBinaryData data(size);

    auto addr = data.data.get();
    // dump所需要的数据
    std::uint64_t current_offset = 0;
    for (const auto &value : values) {
        if (value.isInt()) {
            // 写入数据类型
            std::uint8_t type = DATA_INT;
            std::memcpy(addr + current_offset, &type, sizeof(type));
            current_offset += sizeof(type);

            // 写入数据
            auto v = value.getIntValue();
            std::memcpy(addr + current_offset, &v, sizeof(v));
            current_offset += sizeof(v);
        } else if (value.isFloat()) {
            // 写入数据类型
            std::uint8_t type = DATA_FLOAT;
            std::memcpy(addr + current_offset, &type, sizeof(type));
            current_offset += sizeof(type);

            // 写入数据
            auto v = value.getFloatValue();
            std::memcpy(addr + current_offset, &v, sizeof(v));
            current_offset += sizeof(v);
        } else if (value.isString()) {
            // 写入数据类型
            std::uint8_t type = DATA_STRING;
            std::memcpy(addr + current_offset, &type, sizeof(type));
            current_offset += sizeof(type);

            auto v = value.getStringValue();

            // 写入字符串长度
            std::uint64_t str_length = v.size();
            std::memcpy(addr + current_offset, &str_length, sizeof(str_length));
            current_offset += sizeof(str_length);

            // 写入字符串
            std::memcpy(addr + current_offset, v.c_str(), v.size());
            current_offset += v.size();
        }
    }

    return data;
}

std::vector<ast::SQLExprValue>
    BinaryOperate::load(const storage::SQLBinaryData &data) {
    std::vector<ast::SQLExprValue> res;

    std::uint64_t current_offset = 0;
    while (current_offset < data.size) {
        ast::SQLExprValue item;

        std::uint8_t value_type;
        auto         addr = data.data.get();
        std::memcpy(&value_type, addr + current_offset, sizeof(value_type));
        current_offset += sizeof(value_type);

        if (value_type == DATA_INT) {
            std::int64_t value;
            std::memcpy(&value, addr + current_offset, sizeof(value));
            current_offset += sizeof(value);

            item = ast::SQLExprValue(value);
        } else if (value_type == DATA_FLOAT) {
            double value;
            std::memcpy(&value, addr + current_offset, sizeof(value));
            current_offset += sizeof(value);

            item = ast::SQLExprValue(value);
        } else if (value_type == DATA_STRING) {
            // 读入字符串长度
            std::uint64_t str_length;
            std::memcpy(&str_length, addr + current_offset, sizeof(str_length));
            current_offset += sizeof(str_length);

            // 先读入到vector中，再构造字符串
            std::vector<char> value_seq(str_length);
            std::memcpy(value_seq.data(), addr + current_offset, str_length);
            current_offset += str_length;

            std::string value;
            value.insert(value.begin(), value_seq.begin(), value_seq.end());

            item = ast::SQLExprValue(value);
        }

        res.push_back(item);
    }
    return res;
}
} // namespace minidb::operate
#include <cstring>
#include <vector>

#include "BinaryOperate.h"
#include "SQLBinaryData.h"
#include "SQLExprValue.h"

namespace minidb::operate {

storage::SQLBinaryData
    BinaryOperate::dump(const std::vector<ast::SQLExprValue> &values) {
    storage::SQLBinaryData data;

    std::uint32_t size = 0;

    // 计算二进制数据所需的大小
    for (const auto &value : values) {
        // 数据类型标识
        size += sizeof(std::uint8_t);

        if (value.isInt()) {
            size += sizeof(int);
        } else if (value.isFloat()) {
            size += sizeof(double);
        } else if (value.isString()) {
            // 字符串长度
            size += sizeof(std::uint32_t);
            size += value.getStringValue().size();
        }
    }
    data.size = size;

    data.data = new char[ size ];

    // dump所需要的数据
    std::uint32_t current_addr = 0;
    for (const auto &value : values) {
        if (value.isInt()) {
            // 写入数据类型
            std::uint8_t type = DATA_INT;
            std::memcpy(data.data + current_addr, &type, sizeof(type));
            current_addr += sizeof(type);

            // 写入数据
            auto v = value.getIntValue();
            std::memcpy(data.data + current_addr, &v, sizeof(v));
            current_addr += sizeof(v);
        } else if (value.isFloat()) {
            // 写入数据类型
            std::uint8_t type = DATA_FLOAT;
            std::memcpy(data.data + current_addr, &type, sizeof(type));
            current_addr += sizeof(type);

            // 写入数据
            auto v = value.getFloatValue();
            std::memcpy(data.data + current_addr, &v, sizeof(v));
            current_addr += sizeof(v);
        } else if (value.isString()) {
            // 写入数据类型
            std::uint8_t type = DATA_STRING;
            std::memcpy(data.data + current_addr, &type, sizeof(type));
            current_addr += sizeof(type);

            auto v = value.getStringValue();

            // 写入字符串长度
            std::uint32_t str_length = v.size();
            std::memcpy(data.data + current_addr, &str_length,
                        sizeof(str_length));
            current_addr += sizeof(str_length);

            // 写入字符串
            std::memcpy(data.data + current_addr, v.c_str(), v.size());
            current_addr += v.size();
        }
    }

    return data;
}

std::vector<ast::SQLExprValue>
    BinaryOperate::load(storage::SQLBinaryData data) {
    std::vector<ast::SQLExprValue> res;

    std::uint32_t current_offset = 0;
    while (current_offset < data.size) {
        ast::SQLExprValue item;

        std::uint8_t value_type;
        std::memcpy(&value_type, data.data + current_offset,
                    sizeof(value_type));
        current_offset += sizeof(value_type);

        if (value_type == DATA_INT) {
            std::int32_t value;
            std::memcpy(&value, data.data + current_offset, sizeof(value));
            current_offset += sizeof(value);

            item = ast::SQLExprValue(value);
        } else if (value_type == DATA_FLOAT) {
            double value;
            std::memcpy(&value, data.data + current_offset, sizeof(value));
            current_offset += sizeof(value);

            item = ast::SQLExprValue(value);
        } else if (value_type == DATA_STRING) {
            // 读入字符串长度
            std::uint32_t str_length;
            std::memcpy(&str_length, data.data + current_offset,
                        sizeof(str_length));
            current_offset += sizeof(str_length);

            // 先读入到vector中，再构造字符串
            std::vector<char> value_seq(str_length);
            std::memcpy(value_seq.data(), data.data + current_offset,
                        str_length);
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
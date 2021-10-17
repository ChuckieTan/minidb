#include <cstring>
#include <vector>

#include "BinaryOperate.h"
#include "SQLBinaryData.h"
#include "SQLExprValue.h"

namespace minidb::operate {

SQLBinaryData BiaryOperate::dumps(const std::vector<ast::SQLExprValue> &values) {
    // 计算数据大小
    std::uint32_t size = 0;
    for (const auto &value : values) {
        size += sizeof(std::int8_t);
        if (value.isInt()) {
            size += sizeof(std::int32_t);
        } else if (value.isFloat()) {
            size += sizeof(double);
        } else if (value.isString()) {
            // 加上 字符串长度和 '\0' 后的大小
            size += sizeof(std::int32_t) +
                    value.getStringValue().size() * (sizeof(char) + 1);
        }
    }
    auto data = new char[ size ];

    // 复制内存
    std::uint32_t currentOffset = 0;
    for (const auto &value : values) {
        if (value.isInt()) {
            // 写入数据类型
            std::memcpy(data + currentOffset, &DATA_INT, sizeof(DATA_INT));
            currentOffset += sizeof(DATA_INT);

            std::int32_t v = value.getIntValue();
            std::memcpy(data + currentOffset, &v, sizeof(v));
            currentOffset += sizeof(v);
        } else if (value.isFloat()) {
            // 写入数据类型
            std::memcpy(data + currentOffset, &DATA_FLOAT, sizeof(DATA_FLOAT));
            currentOffset += sizeof(DATA_FLOAT);

            double v = value.getFloatValue();
            std::memcpy(data + currentOffset, &v, sizeof(v));
            currentOffset += sizeof(v);
        } else if (value.isString()) {
            // 写入数据类型
            std::memcpy(data + currentOffset, &DATA_STRING,
                        sizeof(DATA_STRING));
            currentOffset += sizeof(DATA_STRING);

            std::string  v       = value.getStringValue();
            std::int32_t strSize = v.size();

            std::memcpy(data + currentOffset, &strSize, sizeof(strSize));
            currentOffset += sizeof(strSize);

            // 加上 '\0'
            std::memcpy(data + currentOffset, v.c_str(),
                        v.size() * (sizeof(char) + 1));
            currentOffset += v.size() * (sizeof(char) + 1);
        }
    }

    SQLBinaryData res;
    res.data = data;
    res.size = size;
    return res;
};

std::vector<ast::SQLExprValue> BiaryOperate::loads(SQLBinaryData data) {
    std::vector<ast::SQLExprValue> res;

    std::uint32_t currentOffset = 0;
    while (currentOffset < data.size) {
        ast::SQLExprValue item;

        std::uint8_t valueType;
        std::memcpy(&valueType, data.data + currentOffset, sizeof(valueType));
        currentOffset += sizeof(valueType);

        if (valueType == DATA_INT) {
            std::int32_t value;
            std::memcpy(&value, data.data + currentOffset, sizeof(value));
            currentOffset += sizeof(value);

            item = ast::SQLExprValue(value);
        } else if (valueType == DATA_FLOAT) {
            double value;
            std::memcpy(&value, data.data + currentOffset, sizeof(value));
            currentOffset += sizeof(value);

            item = ast::SQLExprValue(value);
        } else if (valueType == DATA_STRING) {
            // 读入字符串长度
            std::uint32_t valueSize;
            std::memcpy(&valueSize, data.data + currentOffset,
                        sizeof(valueSize));
            currentOffset += sizeof(valueSize);

            // 先读入到vector中，再构造字符串
            std::vector<char> valueSeq(valueSize);
            std::memcpy(valueSeq.data(), data.data + currentOffset, valueSize);
            currentOffset += valueSize;

            std::string value;
            value.insert(value.begin(), valueSeq.begin(), valueSeq.end());

            item = ast::SQLExprValue(value);
        }

        res.push_back(item);
    }
    return res;
}
} // namespace minidb::operate
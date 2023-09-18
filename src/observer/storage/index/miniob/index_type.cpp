#include "storage/index/index_type.h"

// 创建字符串到索引类型的映射
std::unordered_map<std::string, IndexType> stringToIndexMap = {
    {"NORMAL", IndexType::IDX_NORMAL},
    {"UNIQUE", IndexType::IDX_UNIQUE},
};

std::unordered_map<IndexType, std::string> indexToStringMap = {
    {IndexType::IDX_NORMAL, "NORMAL"},
    {IndexType::IDX_UNIQUE, "UNIQUE"},
};

IndexType stringToIndex(const std::string& str) {
    auto it = stringToIndexMap.find(str);
    if (it != stringToIndexMap.end()) {
        return it->second;
    } else {
        // 如果字符串无效，则返回默认值
        return IndexType::IDX_NORMAL;
    }
}

// 将枚举值映射回字符串
std::string indexToString(IndexType index) {
    auto it = indexToStringMap.find(index);
    if (it != indexToStringMap.end()) {
        return it->second;
    } else {
        // 处理无效的枚举值
        return "UNKNOWN";
    }
}
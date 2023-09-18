#include "storage/index/index_type.h"

// 创建字符串到索引类型的映射
std::unordered_map<std::string, IndexType> stringToIndexMap = {
    {"NORMAL", IndexType::IDX_NORMAL},
    {"UNIQUE", IndexType::IDX_UNIQUE},
};

IndexType stringToIndex(const std::string& str) {
    auto it = stringToIndexMap.find(str);
    if (it != stringToIndexMap.end()) {
        return it->second;
    } else {
        // 如果字符串无效，则返回默认值或抛出异常，具体情况可以根据需求来处理
        return IndexType::IDX_NORMAL;  // 或者抛出异常
    }
}
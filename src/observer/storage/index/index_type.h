#pragma once

#include <iostream>
#include <string>
#include <unordered_map>

enum class IndexType {
    IDX_NORMAL,
    IDX_UNIQUE
};

extern std::unordered_map<std::string, IndexType> stringToIndexMap;

IndexType stringToIndex(const std::string& str);

std::string indexToString(IndexType index);
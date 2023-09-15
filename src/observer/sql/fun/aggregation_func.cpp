#include "src/observer/sql/fun/aggregation_func.h"

AggregationFunc::AggregationFunc() {
}

// 拷贝构造函数
AggregationFunc::AggregationFunc(const AggregationFunc& other) {
    type_ = other.type_;
}

RC AggregationFunc ::operate() {
    return RC::VARIABLE_NOT_VALID;
}

FuncType stringToFuncType(const std::string& str) {
    static std::map<std::string, FuncType> funcTypeMap = {
        {"MAX", FuncType::FUNC_MAX},
        {"MIN", FuncType::FUNC_MIN},
        {"COUNT", FuncType::FUNC_COUNT},
        {"AVG", FuncType::FUNC_AVG},
        {"UNDIFINED", FuncType::FUNC_UNDIFINED}};

    auto it = funcTypeMap.find(str);
    if (it != funcTypeMap.end()) {
        return it->second;
    } else {
        // 处理无效字符串的情况，例如抛出异常或返回默认值
        return FuncType::FUNC_UNDIFINED;
    }
}
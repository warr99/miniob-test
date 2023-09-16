#include "src/observer/sql/fun/aggregation_func.h"

AggregationFunc::AggregationFunc() {
}

// 拷贝构造函数
AggregationFunc::AggregationFunc(const AggregationFunc& other) {
    type_ = other.type_;
}

RC AggregationFunc ::operate(std::vector<Value>& values) const {
    return RC::VARIABLE_NOT_VALID;
}

std::string toUpperCase(const std::string& str) {
    std::string result = str;  // 创建一个副本以存储结果

    // 使用循环遍历字符串中的每个字符
    for (char& c : result) {
        c = std::toupper(c);  // 将字符转换为大写
    }

    return result;
}

FuncType stringToFuncType(const std::string& str) {
    std::string res = toUpperCase(str);

    static std::map<std::string, FuncType> funcTypeMap = {
        {"MAX", FuncType::FUNC_MAX},
        {"MIN", FuncType::FUNC_MIN},
        {"COUNT", FuncType::FUNC_COUNT},
        {"AVG", FuncType::FUNC_AVG},
        {"UNDIFINED", FuncType::FUNC_UNDIFINED}};

    auto it = funcTypeMap.find(res);
    if (it != funcTypeMap.end()) {
        return it->second;
    } else {
        // 处理无效字符串的情况，例如抛出异常或返回默认值
        return FuncType::FUNC_UNDIFINED;
    }
}

std::string funcTypeToString(FuncType funcType) {
    static std::map<FuncType, std::string> funcTypeStringMap = {
        {FuncType::FUNC_MAX, "MAX"},
        {FuncType::FUNC_MIN, "MIN"},
        {FuncType::FUNC_COUNT, "COUNT"},
        {FuncType::FUNC_AVG, "AVG"},
        {FuncType::FUNC_UNDIFINED, "UNDIFINED"}};

    auto it = funcTypeStringMap.find(funcType);
    if (it != funcTypeStringMap.end()) {
        return it->second;
    } else {
        // 处理无效的FuncType值，例如抛出异常或返回默认字符串
        return "INVALID";
    }
}

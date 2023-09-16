#include "src/observer/sql/fun/avg_func.h"

AvgFunc::AvgFunc() {
    type_ = FuncType::FUNC_AVG;
}

RC AvgFunc::operate(std::vector<Value>& values) const {
    if (values.empty()) {
        return RC::EMPTY;
    }
    // 确保第一个值是数值类型（整数或浮点数）
    AttrType firstValueType = values[0].getType();
    if (firstValueType != AttrType::INTS && firstValueType != AttrType::FLOATS) {
        // 第一个值不是数值类型，返回错误状态
        return RC::EMPTY;
    }
    // 初始化总和和元素个数
    double sum = 0;
    int count = 0;
    // 遍历values中的所有元素，计算总和和元素个数
    for (const Value& val : values) {
        if (val.getType() == AttrType::INTS) {
            sum += static_cast<double>(val.get_int());
        } else if (val.getType() == AttrType::FLOATS) {
            sum += static_cast<double>(val.get_float());
        }
        count++;
    }

    // 计算均值
    double avg = sum / count;
    // 将均值写回到values的第一个值中
    values[0].set_float(static_cast<float>(avg));
    values.erase(values.begin() + 1, values.end());
    return RC::SUCCESS;
}
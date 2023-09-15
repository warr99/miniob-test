#pragma once

#include <map>
#include <stdexcept>
#include <string>
#include "common/rc.h"

enum class FuncType {
    FUNC_MAX,    // 默认值为0
    FUNC_MIN,    // 默认值为1
    FUNC_COUNT,  // 默认值为2
    FUNC_AVG,
    FUNC_UNDIFINED
};

FuncType stringToFuncType(const std::string& str);

class AggregationFunc {
   protected:
    FuncType type_ = FuncType::FUNC_UNDIFINED;

   public:
    virtual RC operate();

    AggregationFunc();

    AggregationFunc(const AggregationFunc& other);

    virtual ~AggregationFunc() = default;

    AggregationFunc& operator=(const AggregationFunc& other) {
        if (this != &other) {  // 避免自赋值
            // 执行合适的赋值操作
            this->type_ = other.type_;
        }
        return *this;
    }

    FuncType getType() {
        return type_;
    }
};



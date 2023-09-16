#include "src/observer/sql/fun/min_func.h"

MinFunc::MinFunc() {
    type_ = FuncType::FUNC_MIN;
}

RC MinFunc::operate(std::vector<Value>& values) const {
    if (values.empty()) {
        return RC::EMPTY;
    }
    std::sort(values.begin(), values.end());
    values.erase(values.begin() + 1, values.end());
    return RC::SUCCESS;
}
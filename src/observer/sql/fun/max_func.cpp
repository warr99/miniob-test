#include "src/observer/sql/fun/max_func.h"

MaxFunc::MaxFunc() {
    type_ = FuncType::FUNC_MAX;
}

RC MaxFunc::operate(std::vector<Value>& values) const {
    if (values.empty()) {
        return RC::EMPTY;
    }
    std::sort(values.begin(), values.end());
    values.erase(values.begin(), std::prev(values.end()));
    return RC::SUCCESS;
}
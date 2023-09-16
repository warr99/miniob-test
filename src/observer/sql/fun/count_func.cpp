#include "src/observer/sql/fun/count_func.h"

CountFunc::CountFunc() {
    type_ = FuncType::FUNC_COUNT;
}

RC CountFunc::operate(std::vector<Value>& values) const {
    if (values.empty()) {
        return RC::EMPTY;
    }
    values[0].set_int(values.size());
    values.erase(values.begin() + 1, values.end());
    return RC::SUCCESS;
}
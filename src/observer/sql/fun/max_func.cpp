#include "src/observer/sql/fun/max_func.h"


MaxFunc::MaxFunc() {
    type_ = FuncType::FUNC_MAX;
}

RC MaxFunc ::operate() {
    // TODO Max 实现
    return RC::SUCCESS;
}
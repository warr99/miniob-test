#pragma once

#include "sql/fun/aggregation_func.h"
#include <algorithm> 

class CountFunc : public AggregationFunc {
   private:
    /* data */
   public:
    CountFunc();
    ~CountFunc() = default;
    RC operate(std::vector<Value>& values) const override;
};


#pragma once

#include "sql/fun/aggregation_func.h"
#include <algorithm> 

class MinFunc : public AggregationFunc {
   private:
    /* data */
   public:
    MinFunc();
    ~MinFunc() = default;
    RC operate(std::vector<Value>& values) const override;
};


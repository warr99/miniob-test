#pragma once

#include "sql/fun/aggregation_func.h"
#include <algorithm> 

class AvgFunc : public AggregationFunc {
   private:
    /* data */
   public:
    AvgFunc();
    ~AvgFunc() = default;
    RC operate(std::vector<Value>& values) const override;
};


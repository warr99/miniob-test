#pragma once

#include "sql/fun/aggregation_func.h"

class MaxFunc : public AggregationFunc {
   private:
    /* data */
   public:
    MaxFunc();
    ~MaxFunc() = default;
    RC operate();
};


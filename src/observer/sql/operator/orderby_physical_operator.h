#pragma once

#include "sql/operator/physical_operator.h"

class OrderByPhysicalOperator : public PhysicalOperator
{
public:
    OrderByPhysicalOperator(const std::vector<OrderSqlNode> &orders) : orders_(orders)
    {}

    virtual ~OrderByPhysicalOperator() = default;

    PhysicalOperatorType type() const override
    {
        return PhysicalOperatorType::ORDERBY;
    }

    RC open(Trx *trx) override;
    RC next() override;
    RC close() override;

    Tuple *current_tuple() override;

private:
    RC scan_query();
    RC order();
    RC order_scan_query();

private:
    std::vector<OrderSqlNode> orders_;
    std::vector<OrderByTuple> tuples_;
    bool is_scan_ = false;
    int current_tuple_index_ = -1;
    size_t max_tuple_num_ = 1000;
    bool is_exceed_max_tuple_num_ = false;
};
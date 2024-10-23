#pragma once

#include <vector>
#include <memory>

#include "sql/operator/logical_operator.h"
#include "sql/expr/expression.h"
#include "storage/field/field.h"
#include "sql/stmt/select_stmt.h"

class OrderByLogicalOperator : public LogicalOperator
{
public:
    OrderByLogicalOperator(const std::vector<OrderSqlNode> &orders);
    virtual ~OrderByLogicalOperator() = default;

    LogicalOperatorType type() const override
    {
        return LogicalOperatorType::ORDERBY;
    }

    std::vector<OrderSqlNode> orders() const
    {
        return orders_;
    }

private:
    std::vector<OrderSqlNode> orders_;
};


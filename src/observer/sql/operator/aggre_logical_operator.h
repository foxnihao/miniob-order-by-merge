#pragma once

#include <vector>
#include <memory>

#include "sql/operator/logical_operator.h"
#include "sql/expr/expression.h"
#include "storage/field/field.h"
#include "sql/stmt/select_stmt.h"

class AggreLogicalOperator : public LogicalOperator
{
public:
    AggreLogicalOperator(const GroupsSqlNode &groups)
        : groups_(groups)
    {}
    virtual ~AggreLogicalOperator() = default;
    LogicalOperatorType type() const override
    {
        return LogicalOperatorType::AGGRE;
    }

    const GroupsSqlNode &groups() const { return groups_; }

private:
    GroupsSqlNode groups_;
};
#pragma once

#include "sql/operator/logical_operator.h"

class UpdateLogicalOperator : public LogicalOperator
{
public:
    UpdateLogicalOperator(Table *table, const std::vector<ParsedUpdateItem> &update_items)
      : table_(table), update_items_(update_items)
    {}
    virtual ~UpdateLogicalOperator() = default;

    LogicalOperatorType type() const override
    {
        return LogicalOperatorType::UPDATE;
    }
    Table *table() const
    {
        return table_;
    }
    const std::vector<ParsedUpdateItem> &update_items() const
    {
        return update_items_;
    }

private:
    Table *table_ = nullptr;
    std::vector<ParsedUpdateItem> update_items_;
};
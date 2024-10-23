#pragma once

#include "sql/operator/physical_operator.h"

class Trx;
class UpdateStmt;

class UpdatePhysicalOperator : public PhysicalOperator
{
public:
    UpdatePhysicalOperator(Table *table, const std::vector<ParsedUpdateItem> &update_items)
      : table_(table), update_items_(update_items)
    {}

    virtual ~UpdatePhysicalOperator() = default;

    PhysicalOperatorType type() const override
    {
        return PhysicalOperatorType::UPDATE;
    }

    RC open(Trx *trx) override;
    RC next() override;
    RC close() override;

    Tuple *current_tuple() override
    {
        return nullptr;
    }

private:
    Table *table_ = nullptr;
    std::vector<ParsedUpdateItem> update_items_;
    Trx *trx_ = nullptr;
};
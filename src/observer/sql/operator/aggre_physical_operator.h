#pragma once

#include "sql/operator/physical_operator.h"
#include "sql/stmt/select_stmt.h"

class AggrePhysicalOperator : public PhysicalOperator
{
public:
    AggrePhysicalOperator(const GroupsSqlNode &groups)
        : groups_(groups)
    {}

    virtual ~AggrePhysicalOperator() = default;

    PhysicalOperatorType type() const override
    {
        return PhysicalOperatorType::AGGRE;
    }

    RC open(Trx *trx) override;
    RC next() override;
    RC close() override;

    RC do_aggre();
    RC get_group_result();

    Tuple *current_tuple() override;

private:
    ValueListTuple tuple_;
    bool done_aggre_ = false;
    GroupsSqlNode groups_;
};
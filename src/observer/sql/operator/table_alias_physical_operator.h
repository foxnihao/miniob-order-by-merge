#pragma once

#include "sql/operator/physical_operator.h"

class TableAliasPhysicalOperator : public PhysicalOperator
{
public:
  TableAliasPhysicalOperator(const std::unordered_map<std::string, std::string> &table_alias_map)
    : tuple_(table_alias_map)
  {}

  virtual ~TableAliasPhysicalOperator() = default;

  PhysicalOperatorType type() const override
  {
    return PhysicalOperatorType::TABLE_ALIAS;
  }

  RC open(Trx *trx) override
  {
    if (children_.empty()) {
        return RC::SUCCESS;
    }

    PhysicalOperator *child = children_[0].get();
    RC rc = child->open(trx);
    if (rc != RC::SUCCESS) {
        LOG_WARN("failed to open child operator: %s", strrc(rc));
        return rc;
    }

    return RC::SUCCESS;
  }
  RC next() override
  {
    if (children_.empty()) {
        return RC::RECORD_EOF;
    }
    return children_[0]->next();
  }
  RC close() override
  {
    if (!children_.empty()) {
        children_[0]->close();
    }
    return RC::SUCCESS;
  }

  Tuple *current_tuple() override
  {
    tuple_.set_tuple(children_[0]->current_tuple());
    return &tuple_;
  }

private:
  TableAliasTuple tuple_;
};

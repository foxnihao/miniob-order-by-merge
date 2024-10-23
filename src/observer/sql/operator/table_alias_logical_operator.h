#pragma once

#include <vector>
#include <memory>

#include "sql/operator/logical_operator.h"
#include "sql/expr/expression.h"
#include "storage/field/field.h"

class TableAliasLogicalOperator : public LogicalOperator 
{
public:
  TableAliasLogicalOperator(const std::unordered_map<std::string, std::string> &table_alias_map)
    : table_alias_map_(table_alias_map) {}
  virtual ~TableAliasLogicalOperator() = default;

  LogicalOperatorType type() const override
  {
    return LogicalOperatorType::TABLE_ALIAS;
  }

  const std::unordered_map<std::string, std::string> &table_alias_map() const
  {
    return table_alias_map_;
  }

private:
  const std::unordered_map<std::string, std::string> &table_alias_map_;
};

#include "sql/operator/orderby_logical_operator.h"

OrderByLogicalOperator::OrderByLogicalOperator(const std::vector<OrderSqlNode> &orders) : orders_(orders)
{}
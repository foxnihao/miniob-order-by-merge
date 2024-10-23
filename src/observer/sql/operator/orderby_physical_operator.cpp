#include "sql/operator/orderby_physical_operator.h"
#include "common/log/log.h"
#include "storage/record/record.h"
#include "storage/table/table.h"

RC OrderByPhysicalOperator::open(Trx *trx)
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

RC OrderByPhysicalOperator::next()
{
  if(children_.empty()) {
    return RC::RECORD_EOF;
  }
  RC rc =RC::SUCCESS;
  if(!is_scan_){
    rc = order_scan_query();
    if(rc != RC::SUCCESS){
      return rc;
    }
  }
  int tuple_num = tuples_.size()-1;
  if(current_tuple_index_ >= tuple_num){
    return RC::RECORD_EOF;
  }
  current_tuple_index_++;
  return RC::SUCCESS;
}

RC OrderByPhysicalOperator::scan_query()
{
  RC rc = RC::SUCCESS;
  while(RC::SUCCESS == children_[0]->next()){
    Tuple *tuple = children_[0]->current_tuple();

    // 求该行 tuple 所有列的值，并存入 ValueListTuple
    std::vector<Value> cell_values;
    for (int i = 0; i < tuple->cell_num(); ++i) {
      Value tuple_cell_value;
      rc = tuple->cell_at(i, tuple_cell_value);
      if (rc != RC::SUCCESS) return rc;
      cell_values.push_back(tuple_cell_value);
    }
    ValueListTuple cell_values_tuple;
    cell_values_tuple.set_cells(cell_values);

    // 求该行对应 orders_ 中所有 expression 的值
    std::vector<Value> order_key_values;
    for (const OrderSqlNode &order : orders_) {
      Value order_key_value;
      rc = order.expression->get_value(*tuple, order_key_value);
      if (rc != RC::SUCCESS) return rc;
      order_key_values.push_back(order_key_value);
    }

    // 将上述两者合并到 OrderByTuple，并保存起来，用来排序
    tuples_.push_back(OrderByTuple(cell_values_tuple, order_key_values));

#if false
    if(tuples_.size()>=max_tuple_num_){
      is_exceed_max_tuple_num_ = true;
      break;
    }
#endif
  }
  is_scan_ = true;
  return RC::SUCCESS;
}

RC OrderByPhysicalOperator::order()
{
  RC rc = RC::SUCCESS;
  if (orders_.empty()) {
    return RC::SUCCESS;
  }
  std::sort(tuples_.begin(), tuples_.end(), [&](OrderByTuple &a, OrderByTuple &b) {
    for (int i = 0; i < orders_.size(); ++i) {
      const OrderSqlNode &order = orders_[i];
      bool is_asc = order.flag == OrderFlag::ORDER_ASC;
      Value cell_a = a.order_key_values()[i];
      Value cell_b = b.order_key_values()[i];
      int cmp_int;
      if (cell_a.is_null() && cell_b.is_null()) {
        cmp_int = 0;
      } else if (cell_a.is_null()) {
        cmp_int = -1;
      } else if (cell_b.is_null()) {
        cmp_int = 1;
      } else {
        Value cmp_val;
        rc = cell_a.compare(cell_b, cmp_val);
        if (rc != RC::SUCCESS || cmp_val.is_null()) {
          return false;
        }
        cmp_int = cmp_val.get_int();
      }
      if (cmp_int == 0) {
        continue;
      }
      if (is_asc) {
        return cmp_int < 0;
      } else {
        return cmp_int > 0;
      }
    }
    return false;
  });
  return RC::SUCCESS;
}

RC OrderByPhysicalOperator::order_scan_query()
{
  RC rc = scan_query();
  if(rc != RC::SUCCESS){
    return rc;
  }
  rc = order();
  if(rc != RC::SUCCESS){
    return rc;
  }
  if(is_exceed_max_tuple_num_){
    return RC::SUCCESS;
  }
  return RC::SUCCESS;
  
}


RC OrderByPhysicalOperator::close()
{
  if (!children_.empty()) {
    children_[0]->close();
  }
  return RC::SUCCESS;
}

Tuple *OrderByPhysicalOperator::current_tuple()
{
  return &tuples_[current_tuple_index_];
}

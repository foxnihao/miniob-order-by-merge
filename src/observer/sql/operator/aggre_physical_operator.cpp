#include "common/log/log.h"
#include "sql/operator/aggre_physical_operator.h"
#include "storage/record/record.h"
#include "storage/table/table.h"
#include "sql/parser/value.h"
#include "sql/parser/parse_defs.h"

RC AggrePhysicalOperator::open(Trx *trx)
{
  if (children_.empty()) {
    return RC::SUCCESS;
  }

  for(int i=0;i<this->children_.size();i++){
    PhysicalOperator *child = children_[i].get();
    RC rc = child->open(trx);
    if (rc != RC::SUCCESS) {
        LOG_WARN("failed to open child operator: %s", strrc(rc));
        return rc;
    }
  }
  
  return RC::SUCCESS;
}

RC AggrePhysicalOperator::next()
{
  RC rc = RC::SUCCESS;

  if (!done_aggre_) {
    rc = do_aggre();
    if (rc != RC::SUCCESS) {
      return rc;
    }
    done_aggre_ = true;
  }

  bool passed;
  do {
    rc = get_group_result();
    if (rc != RC::SUCCESS) {
      return rc;
    }
    if (groups_.having) {
      // 根据 having 条件过滤
      Value value;
      rc = groups_.having->try_get_value(value); // try_get_value 让 having 的聚合函数表达式与查询表达式同步取结果
      if (rc != RC::SUCCESS) {
        return rc;
      }
      bool bool_val = value.is_null() ? false : value.get_boolean();
      passed = !bool_val;
    } else {
      passed = false;
    }
  } while (passed);

  return rc;
}

RC AggrePhysicalOperator::get_group_result()
{
  RC rc = RC::SUCCESS;

  if (children_.empty()) {
    return RC::RECORD_EOF;
  }
  PhysicalOperator *oper = children_.front().get();

  // 取出一个 group 的聚合函数计算结果

  // 这一步 static_cast 的前提是下层 operator 返回的 tuple 类型一定是 ProjectTuple
  ProjectTuple *tuple = static_cast<ProjectTuple *>(oper->current_tuple());

  std::vector<Value> values;
  for (int i = 0; i < tuple->cell_num(); ++i) {
    Value cell_value;
    Expression *cell_expr;
    rc = tuple->try_cell_at(i, cell_value, cell_expr); // try_cell_at 会调用到聚合函数表达式的 try_get_value，
                                                       // 取出其当前组结果，并迭代其组号
    if (rc != RC::SUCCESS) {
      return rc;
    }
    values.push_back(cell_value);
  }
  tuple_.set_cells(values);

  return rc;
}

RC AggrePhysicalOperator::do_aggre()
{
  RC rc = RC::SUCCESS;

  if (children_.empty()) {
    return RC::RECORD_EOF;
  }
  PhysicalOperator *oper = children_.front().get();

  while (RC::SUCCESS == (rc = oper->next())) {
    Tuple *tuple = oper->current_tuple();

    // 调用所有查询表达式的 get_value，为 AggreExpr 累积结果，只要是聚合函数，tmp_value 应该就会得到空值
    for (int i = 0; i < tuple->cell_num(); ++i) {
      Value tmp_value;
      rc = tuple->cell_at(i, tmp_value);
      if (rc != RC::SUCCESS) {
        break;
      }
    }
    // 调用 having 条件的 get_value，同理
    if (groups_.having) {
      Value tmp_value;
      rc = groups_.having->get_value(*tuple, tmp_value);
      if (rc != RC::SUCCESS) {
        break;
      }
    }
  }
  if (rc != RC::RECORD_EOF) {
    return rc;
  }

  return RC::SUCCESS;
}

RC AggrePhysicalOperator::close()
{
  for(int i=0;i<this->children_.size();i++){
    PhysicalOperator *child = children_[0].get();
    RC rc = child->close();
    if (rc != RC::SUCCESS) {
        LOG_WARN("failed to close child operator: %s", strrc(rc));
        return rc;
    }
  }
  return RC::SUCCESS;
}

Tuple *AggrePhysicalOperator::current_tuple()
{
  return &tuple_;
}

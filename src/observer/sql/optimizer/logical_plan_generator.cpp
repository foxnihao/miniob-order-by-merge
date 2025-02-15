/* Copyright (c) 2023 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2023/08/16.
//

#include "sql/optimizer/logical_plan_generator.h"

#include "sql/operator/logical_operator.h"
#include "sql/operator/calc_logical_operator.h"
#include "sql/operator/project_logical_operator.h"
#include "sql/operator/predicate_logical_operator.h"
#include "sql/operator/table_get_logical_operator.h"
#include "sql/operator/table_alias_logical_operator.h"
#include "sql/operator/insert_logical_operator.h"
#include "sql/operator/delete_logical_operator.h"
#include "sql/operator/update_logical_operator.h"
#include "sql/operator/join_logical_operator.h"
#include "sql/operator/project_logical_operator.h"
#include "sql/operator/explain_logical_operator.h"
#include "sql/operator/aggre_logical_operator.h"
#include "sql/operator/orderby_logical_operator.h"

#include "sql/stmt/stmt.h"
#include "sql/stmt/calc_stmt.h"
#include "sql/stmt/select_stmt.h"
#include "sql/stmt/filter_stmt.h"
#include "sql/stmt/insert_stmt.h"
#include "sql/stmt/delete_stmt.h"
#include "sql/stmt/update_stmt.h"
#include "sql/stmt/explain_stmt.h"

using namespace std;

RC LogicalPlanGenerator::create(Stmt *stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  RC rc = RC::SUCCESS;
  switch (stmt->type()) {
    case StmtType::CALC: {
      CalcStmt *calc_stmt = static_cast<CalcStmt *>(stmt);
      rc = create_plan(calc_stmt, logical_operator);
    } break;

    case StmtType::SELECT: {
      SelectStmt *select_stmt = static_cast<SelectStmt *>(stmt);
      rc = create_plan(select_stmt, logical_operator);
    } break;

    case StmtType::INSERT: {
      InsertStmt *insert_stmt = static_cast<InsertStmt *>(stmt);
      rc = create_plan(insert_stmt, logical_operator);
    } break;

    case StmtType::UPDATE: {
      UpdateStmt *update_stmt = static_cast<UpdateStmt *>(stmt);
      rc = create_plan(update_stmt, logical_operator);
    } break;

    case StmtType::DELETE: {
      DeleteStmt *delete_stmt = static_cast<DeleteStmt *>(stmt);
      rc = create_plan(delete_stmt, logical_operator);
    } break;

    case StmtType::EXPLAIN: {
      ExplainStmt *explain_stmt = static_cast<ExplainStmt *>(stmt);
      rc = create_plan(explain_stmt, logical_operator);
    } break;
    default: {
      rc = RC::UNIMPLENMENT;
    }
  }
  return rc;
}

RC LogicalPlanGenerator::create_plan(CalcStmt *calc_stmt, std::unique_ptr<LogicalOperator> &logical_operator)
{
  logical_operator.reset(new CalcLogicalOperator(std::move(calc_stmt->expressions())));
  return RC::SUCCESS;
}

RC LogicalPlanGenerator::create_plan(
    SelectStmt *select_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  RC rc = RC::SUCCESS;
  
  const std::vector<Table *> &tables = select_stmt->tables();
  const std::vector<Expression *> &join_conditions = select_stmt->join_conditions();

  unique_ptr<LogicalOperator> table_oper(nullptr);
  std::vector<std::unique_ptr<Expression>> &all_exprs = select_stmt->expressions();
  for (int i = 0; i < tables.size(); ++i) {
    Table *table = tables[i];
    Expression *join_condition = join_conditions[i];
    // 把每个表单独拿出来创建table_get_oper
    unique_ptr<LogicalOperator> table_get_oper(new TableGetLogicalOperator(table, std::vector<Field>(), true/*readonly*/));
    if (table_oper == nullptr) {
      table_oper = std::move(table_get_oper);
    } else {
      JoinLogicalOperator *join_oper = new JoinLogicalOperator(join_condition);
      join_oper->add_child(std::move(table_oper));
      join_oper->add_child(std::move(table_get_oper));
      table_oper = unique_ptr<LogicalOperator>(join_oper);
    }
  }

  if (table_oper == nullptr) {
    // FROM 为空，输出单行查询结果，故 table_oper 可用符合该要求的 CalcLogicalOperator 充数
    std::vector<std::unique_ptr<Expression>> exprs;
    exprs.push_back(std::unique_ptr<Expression>(new ValueExpr(Value(0))));
    table_oper = unique_ptr<LogicalOperator>(new CalcLogicalOperator(std::move(exprs)));
  }

  unique_ptr<LogicalOperator> table_alias_oper(new TableAliasLogicalOperator(select_stmt->table_alias_map()));
  table_alias_oper->add_child(std::move(table_oper));
  table_oper.swap(table_alias_oper);

  unique_ptr<LogicalOperator> predicate_oper(nullptr);
  if (select_stmt->filter_stmt()) {
    rc = create_plan(select_stmt->filter_stmt(), predicate_oper);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create predicate logical plan. rc=%s", strrc(rc));
      return rc;
    }
  }

  unique_ptr<LogicalOperator> project_oper(new ProjectLogicalOperator(std::move(all_exprs),
                                                                      select_stmt->outer_query_tuple));
  if (predicate_oper) {
    if (table_oper) {
      project_oper->add_child(std::move(table_oper));
    }
    // 先投影（并在 find_cell 中加入对表中字段的查找，参考 ProjectTuple 的改写），再筛选，因为筛选既需要表中字段，也需要表达式
    // 上述无效，因为比赛不让我支持这个，所以我又改回来了
    // 上述再次无效，因为换了个思路，我又改回去了
    predicate_oper->add_child(std::move(project_oper));
  } else {
    if (table_oper) {
      project_oper->add_child(std::move(table_oper));
    }
  }

  if (predicate_oper) logical_operator.swap(predicate_oper);
  else logical_operator.swap(project_oper);

  const auto &groups = select_stmt->groups();
  if (select_stmt->has_aggre() || !groups.group_exprs.empty()) {
    unique_ptr<LogicalOperator> aggre_oper(new AggreLogicalOperator(groups));
    aggre_oper->add_child(std::move(logical_operator));
    logical_operator.swap(aggre_oper);
  }

  const std::vector<OrderSqlNode> orders = select_stmt->orders();
  if (!orders.empty()) {
    unique_ptr<LogicalOperator> order_oper(new OrderByLogicalOperator(orders));
    order_oper->add_child(std::move(logical_operator));
    logical_operator.swap(order_oper);
  }
  
  return RC::SUCCESS;
}

RC LogicalPlanGenerator::create_plan(
    FilterStmt *filter_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  unique_ptr<PredicateLogicalOperator> predicate_oper;
  if (filter_stmt->expression() != nullptr) {
    predicate_oper = unique_ptr<PredicateLogicalOperator>(new PredicateLogicalOperator(std::move(filter_stmt->expression())));
  }

  logical_operator = std::move(predicate_oper);
  return RC::SUCCESS;
}

RC LogicalPlanGenerator::create_plan(
    InsertStmt *insert_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  Table *table = insert_stmt->table();
  vector<Value> *values = new vector<Value>(insert_stmt->values(), insert_stmt->values() + insert_stmt->value_amount());

  InsertLogicalOperator *insert_operator = new InsertLogicalOperator(table, values);
  logical_operator.reset(insert_operator);
  return RC::SUCCESS;
}

RC LogicalPlanGenerator::create_plan(
    DeleteStmt *delete_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  RC rc = RC::SUCCESS;

  Table *table = delete_stmt->table();
  FilterStmt *filter_stmt = delete_stmt->filter_stmt();
  std::vector<Field> fields;
  for (int i = table->table_meta().sys_field_num(); i < table->table_meta().field_num(); i++) {
    const FieldMeta *field_meta = table->table_meta().field(i);
    fields.push_back(Field(table, field_meta));
  }
  unique_ptr<LogicalOperator> table_oper(new TableGetLogicalOperator(table, fields, false/*readonly*/));

  unique_ptr<LogicalOperator> table_alias_oper(new TableAliasLogicalOperator(delete_stmt->table_alias_map()));
  table_alias_oper->add_child(std::move(table_oper));
  table_oper.swap(table_alias_oper);

  unique_ptr<LogicalOperator> predicate_oper(nullptr);
  if (delete_stmt->filter_stmt()) {
    rc = create_plan(filter_stmt, predicate_oper);
    if (rc != RC::SUCCESS) {
      return rc;
    }
  }

  unique_ptr<LogicalOperator> delete_oper(new DeleteLogicalOperator(table));

  if (predicate_oper) {
    predicate_oper->add_child(std::move(table_oper));
    delete_oper->add_child(std::move(predicate_oper));
  } else {
    delete_oper->add_child(std::move(table_oper));
  }

  logical_operator = std::move(delete_oper);
  return rc;
}

RC LogicalPlanGenerator::create_plan(
    UpdateStmt *update_stmt, std::unique_ptr<LogicalOperator> &logical_operator)
{
  RC rc = RC::SUCCESS;

  Table *table = update_stmt->table();
  FilterStmt *filter_stmt = update_stmt->filter_stmt();
  const std::vector<ParsedUpdateItem> &update_items = update_stmt->update_items();

  unique_ptr<LogicalOperator> table_oper(new TableGetLogicalOperator(table, std::vector<Field>(), false/*readonly*/));

  unique_ptr<LogicalOperator> table_alias_oper(new TableAliasLogicalOperator(update_stmt->table_alias_map()));
  table_alias_oper->add_child(std::move(table_oper));
  table_oper.swap(table_alias_oper);

  unique_ptr<LogicalOperator> predicate_oper;
  if (update_stmt->filter_stmt()) {
    rc = create_plan(filter_stmt, predicate_oper);
    if (rc != RC::SUCCESS) {
      return rc;
    }
  }

  unique_ptr<LogicalOperator> update_oper(new UpdateLogicalOperator(table, update_items));

  if (predicate_oper) {
    predicate_oper->add_child(std::move(table_oper));
    update_oper->add_child(std::move(predicate_oper));
  } else {
    update_oper->add_child(std::move(table_oper));
  }

  logical_operator = std::move(update_oper);
  return rc;
}

RC LogicalPlanGenerator::create_plan(
    ExplainStmt *explain_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  Stmt *child_stmt = explain_stmt->child();
  unique_ptr<LogicalOperator> child_oper;
  RC rc = create(child_stmt, child_oper);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create explain's child operator. rc=%s", strrc(rc));
    return rc;
  }

  logical_operator = unique_ptr<LogicalOperator>(new ExplainLogicalOperator);
  logical_operator->add_child(std::move(child_oper));
  return rc;
}

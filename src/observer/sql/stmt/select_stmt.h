/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2022/6/5.
//

#pragma once

#include <vector>
#include <memory>

#include "common/rc.h"
#include "sql/stmt/stmt.h"
#include "storage/field/field.h"
#include "sql/expr/expression.h"

class FieldMeta;
class FilterStmt;
class Db;
class Table;
class Tuple;


/**
 * @brief 表示select语句
 * @ingroup Statement
 */
class SelectStmt : public Stmt 
{
public:
  SelectStmt() = default;
  ~SelectStmt() override;

  StmtType type() const override
  {
    return StmtType::SELECT;
  }

public:
  static RC create(Session *session, Db *db, const SelectSqlNode &select_sql, Stmt *&stmt,
                      const std::unordered_map<std::string, Table *> *merged_outer_queries_table_map = nullptr,
                      const std::vector<ExprInfo> *merged_outer_queries_expr_infos = nullptr);

public:
  std::vector<std::unique_ptr<Expression>> &expressions()
  {
    return expressions_;
  }
  const std::vector<std::unique_ptr<Expression>> &expressions() const
  {
    return expressions_;
  }
  const std::vector<ExprInfo> &expr_infos() const
  {
    return expr_infos_;
  }
  std::unordered_map<std::string, std::string> &table_alias_map()
  {
    return table_alias_map_;
  }
  const std::unordered_map<std::string, std::string> &table_alias_map() const
  {
    return table_alias_map_;
  }
  const std::vector<Table *> &tables() const
  {
    return tables_;
  }
  const std::vector<Expression *> &join_conditions() const
  {
    return join_conditions_;
  }
  FilterStmt *filter_stmt() const
  {
    return filter_stmt_;
  }
  bool has_aggre() const
  {
    return has_aggre_;
  }
  const GroupsSqlNode &groups() const
  {
    return groups_;
  }
  const std::vector<OrderSqlNode> &orders() const
  {
    return orders_;
  }

private:
  std::vector<std::unique_ptr<Expression>> expressions_;
  std::vector<ExprInfo> expr_infos_;  // 另存表达式名字等信息，因为表达式的 unique_ptr 被转移之后
                                      // execute_stage.cpp  仍会用到它们的名字；
                                      // create table as 也会用到子查询的信息
  std::vector<Table *> tables_;
  std::vector<Expression *> join_conditions_;
  std::unordered_map<std::string, std::string> table_alias_map_;
  FilterStmt *filter_stmt_ = nullptr;
  bool has_aggre_ = false;
  GroupsSqlNode groups_;
  std::vector<OrderSqlNode> orders_;

public:
  const Tuple *outer_query_tuple = nullptr;
};

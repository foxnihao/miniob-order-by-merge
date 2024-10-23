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
// Created by Wangyunlai on 2022/5/27.
//

#pragma once

#include <unordered_map>
#include "sql/stmt/stmt.h"
#include "sql/parser/parse_defs.h"

class Table;
class FilterStmt;

/**
 * @brief Delete 语句
 * @ingroup Statement
 */
class DeleteStmt : public Stmt 
{
public:
  DeleteStmt(Table *table, FilterStmt *filter_stmt);
  ~DeleteStmt() override;

  Table *table() const
  {
    return table_;
  }
  FilterStmt *filter_stmt() const
  {
    return filter_stmt_;
  }

  StmtType type() const override
  {
    return StmtType::DELETE;
  }

  std::unordered_map<std::string, std::string> &table_alias_map()
  {
    return table_alias_map_;
  }
  const std::unordered_map<std::string, std::string> &table_alias_map() const
  {
    return table_alias_map_;
  }

public:
  static RC create(Session *session, Db *db, const DeleteSqlNode &delete_sql, Stmt *&stmt);

private:
  Table *table_ = nullptr;
  std::unordered_map<std::string, std::string> table_alias_map_;
  FilterStmt *filter_stmt_ = nullptr;
};

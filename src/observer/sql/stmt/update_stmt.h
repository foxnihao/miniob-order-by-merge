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
// Created by Wangyunlai on 2022/5/22.
//

#pragma once

#include <unordered_map>
#include "common/rc.h"
#include "sql/stmt/stmt.h"

class Table;
class FilterStmt;
class FieldMeta;
class Field;  

/**
 * @brief 更新语句
 * @ingroup Statement
 */
class UpdateStmt : public Stmt 
{
public:
  UpdateStmt() = default;

  ~UpdateStmt() override;

public:
  static RC create(Session *session, Db *db, const UpdateSqlNode &update_sql, Stmt *&stmt);

public:
  Table *table() const
  {
    return table_;
  }

  FilterStmt *filter_stmt() const
  {
    return filter_stmt_;
  }

  const std::vector<ParsedUpdateItem> &update_items() const
  {
    return update_items_;
  }

  StmtType type() const override
  {
    return StmtType::UPDATE;
  }

  std::unordered_map<std::string, std::string> &table_alias_map()
  {
    return table_alias_map_;
  }
  const std::unordered_map<std::string, std::string> &table_alias_map() const
  {
    return table_alias_map_;
  }

private:
  Table *table_ = nullptr;
  std::vector<ParsedUpdateItem> update_items_;
  std::unordered_map<std::string, std::string> table_alias_map_;
  FilterStmt *filter_stmt_ = nullptr;
};

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

#include "common/log/log.h"
#include "sql/stmt/delete_stmt.h"
#include "sql/stmt/filter_stmt.h"
#include "storage/db/db.h"
#include "storage/table/table.h"

DeleteStmt::DeleteStmt(Table *table, FilterStmt *filter_stmt) : table_(table), filter_stmt_(filter_stmt)
{}

DeleteStmt::~DeleteStmt()
{
  if (nullptr != filter_stmt_) {
    delete filter_stmt_;
    filter_stmt_ = nullptr;
  }
}

RC DeleteStmt::create(Session *session, Db *db, const DeleteSqlNode &delete_sql, Stmt *&stmt)
{
  RC rc = RC::SUCCESS;

  const char *table_name = delete_sql.relation_name.c_str();
  if (nullptr == db || nullptr == table_name) {
    LOG_WARN("invalid argument. db=%p, table_name=%p", db, table_name);
    return RC::INVALID_ARGUMENT;
  }

  // check whether the table exists
  Table *table = db->find_table(table_name);
  if (nullptr == table) {
    LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  const char *table_alias = delete_sql.relation_alias.c_str();

  std::unordered_map<std::string, Table *> table_map;
  std::unordered_map<std::string, std::string> table_alias_map;
  table_map.insert(std::pair<std::string, Table *>(std::string(table_alias), table));
  table_map.insert(std::pair<std::string, Table *>("", table));
  table_alias_map.insert(std::pair<std::string, std::string>(std::string(table_alias), std::string(table_name)));
  table_alias_map.insert(std::pair<std::string, std::string>("", std::string(table_name)));

  std::unique_ptr<Expression> condition(delete_sql.condition);
  if (condition != nullptr) {
    std::vector<ExprInfo> expr_infos_empty;
    rc = condition->init_expr_tree(session, db, nullptr, table_map, expr_infos_empty);
    if (rc != RC::SUCCESS) {
      return rc;
    }
  }

  FilterStmt *filter_stmt = nullptr;
  if (condition) {
    rc = FilterStmt::create(session, db, std::move(condition), filter_stmt);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create filter statement. rc=%d:%s", rc, strrc(rc));
      return rc;
    }
  }

  DeleteStmt *del_stmt = new DeleteStmt(table, filter_stmt);
  del_stmt->table_alias_map_.swap(table_alias_map);
  stmt = del_stmt;
  return rc;
}

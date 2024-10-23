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

#include "sql/stmt/update_stmt.h"

#include "common/log/log.h"
#include "sql/stmt/filter_stmt.h"
#include "storage/db/db.h"
#include "storage/table/table.h"
#include "storage/table/table_meta.h"
#include "storage/field/field_meta.h"

UpdateStmt::~UpdateStmt()
{
  
}

RC UpdateStmt::create(Session *session, Db *db, const UpdateSqlNode &update, Stmt *&stmt)
{
  RC rc = RC::SUCCESS;

  // TODO
  const char *table_name = update.relation_name.c_str();
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

  // check the 'where' clause
  const char *table_alias = update.relation_alias.c_str();

  std::unordered_map<std::string, Table *> table_map;
  std::unordered_map<std::string, std::string> table_alias_map;
  table_map.insert(std::pair<std::string, Table *>(std::string(table_alias), table));
  table_map.insert(std::pair<std::string, Table *>("", table));
  table_alias_map.insert(std::pair<std::string, std::string>(std::string(table_alias), std::string(table_name)));
  table_alias_map.insert(std::pair<std::string, std::string>("", std::string(table_name)));

  std::unique_ptr<Expression> condition(update.condition);
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

  std::vector<ParsedUpdateItem> processed_update_items;
  for (const auto &item : update.update_items) {
    const char *field_name = item->attribute_name.c_str();
    Expression *expression = item->expression;
    if (nullptr == field_name) {
      LOG_WARN("invalid argument. field_name=%p", field_name);
      return RC::INVALID_ARGUMENT;
    }

    // check whether the field exists
    const FieldMeta *field_meta = table->table_meta().field(field_name);
    if (nullptr == field_meta) {
      LOG_WARN("no such field. table_name=%s, field_name=%s", table_name, field_name);
      return RC::SCHEMA_FIELD_NOT_EXIST;
    }

    if (expression->type() != ExprType::VALUE) {
      std::unordered_map<std::string, Table *> table_map_empty;
      std::vector<ExprInfo> expr_infos_empty;
      rc = expression->init_expr_tree(session, db, nullptr, table_map_empty, expr_infos_empty);
      if (rc != RC::SUCCESS) {
        return rc;
      }
    }

    ParsedUpdateItem processed_item; // item 预处理结果

    processed_item.attribute_name = field_name;
    processed_item.field = new Field(table, field_meta);

    // 将表达式求值
    if (item->expression->type() == ExprType::VALUE) {
      processed_item.value = static_cast<ValueExpr *>(item->expression)->get_value();
    } else {
      rc = item->expression->try_get_value(processed_item.value);
      if (rc != RC::SUCCESS) {
        return rc;
      }
    }
    delete item->expression;

    processed_update_items.push_back(processed_item);
  }

  UpdateStmt *upd_stmt = new UpdateStmt();
  upd_stmt->table_ = table;
  upd_stmt->update_items_.swap(processed_update_items);
  upd_stmt->filter_stmt_ = filter_stmt;
  upd_stmt->table_alias_map_.swap(table_alias_map);
  stmt = upd_stmt;
  return rc;
}

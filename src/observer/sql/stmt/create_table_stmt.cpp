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
// Created by Wangyunlai on 2023/6/13.
//

#include <unordered_set>
#include "sql/stmt/create_table_stmt.h"
#include "sql/expr/expression.h"
#include "event/sql_debug.h"
#include "sql/parser/parse.h"

RC CreateTableStmt::create(Session *session, Db *db, const CreateTableSqlNode &create_table, Stmt *&stmt)
{
  RC rc = RC::SUCCESS;

  CreateTableStmt *create_table_stmt;
  if (create_table.as_expression) {
    create_table_stmt = new CreateTableStmt(create_table.relation_name);

    if (create_table.as_expression->type() != ExprType::SUBQUERY) {
      return RC::INTERNAL;
    }

    SubqueryExpr *subquery_expr = static_cast<SubqueryExpr *>(create_table.as_expression);

    // 传 session 和 db 参数
    std::unordered_map<std::string, Table *> table_map_empty;
    std::vector<ExprInfo> expr_infos_empty;
    rc = subquery_expr->init_expr_tree(session, db, nullptr, table_map_empty, expr_infos_empty);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to init join condition expr tree");
      return rc;
    }

    // 执行子查询并将结果存入 create_table_stmt
    create_table_stmt->subquery_result_ = new Value();
    rc = subquery_expr->try_get_value(*create_table_stmt->subquery_result_);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to get subquery result when creating table as subquery");
      return rc;
    }

    std::vector<AttrInfoSqlNode> attr_infos;
    const auto &expr_infos = subquery_expr->expr_infos();
    if (create_table.attr_infos.size() == 0) {
      // 推断 attr_infos，并检查重名
      std::unordered_set<std::string> names;
      for (const auto &expr_info : expr_infos) {
        AttrInfoSqlNode attr_info;
        std::string name = expr_info.short_name;
        if (names.count(name) != 0) {
          LOG_WARN("duplicate column names: %s", name.c_str());
          return RC::INVALID_ARGUMENT;
        }
        names.insert(name);
        attr_info.name = name;
        attr_info.type = expr_info.value_type;
        attr_info.is_null = expr_info.inferred_nullable;
        attr_info.length = expr_info.inferred_length;
        attr_infos.push_back(attr_info);
      }
    } else {
      // 检查 attr_infos 和 expr_infos 的一致性
      attr_infos = create_table.attr_infos;
      if (attr_infos.size() != expr_infos.size()) {
        LOG_WARN("new table's number of columns is not equal to that of the subquery expression");
        return RC::INVALID_ARGUMENT;
      }
      for (int i = 0; i < attr_infos.size(); ++i) {
        const AttrInfoSqlNode &attr_info = attr_infos[i];
        const ExprInfo &expr_info = expr_infos[i];
        //if (!Value::can_convert(expr_info.value_type, attr_info.type)) {
        if (expr_info.value_type != attr_info.type) {
          return RC::SCHEMA_FIELD_TYPE_MISMATCH;
        }
      }
    }
    create_table_stmt->attr_infos_.swap(attr_infos);

  } else {
    create_table_stmt = new CreateTableStmt(create_table.relation_name, create_table.attr_infos);
  }

  stmt = create_table_stmt;
  sql_debug("create table statement: table name %s", create_table.relation_name.c_str());
  return RC::SUCCESS;
}

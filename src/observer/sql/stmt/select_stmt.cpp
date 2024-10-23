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
// Created by Wangyunlai on 2022/6/6.
//

#include "sql/expr/expression.h"
#include "sql/stmt/select_stmt.h"
#include "sql/stmt/filter_stmt.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "storage/db/db.h"
#include "storage/table/table.h"

SelectStmt::~SelectStmt()
{
  /*if (nullptr != filter_stmt_) {
    delete filter_stmt_;
    filter_stmt_ = nullptr;
  }*/
}

/*static void wildcard_fields(Table *table, std::vector<Field> &field_metas, std::vector<std::string> &cell_aliases,
                            bool aliases_with_table_name)
{
  const TableMeta &table_meta = table->table_meta();
  const int field_num = table_meta.field_num();
  for (int i = table_meta.sys_field_num(); i < field_num; i++) {
    field_metas.push_back(Field(table, table_meta.field(i)));
    if (aliases_with_table_name) {
      cell_aliases.push_back(std::string(table_meta.name()) + "." + std::string(table_meta.field(i)->name()));
    } else {
      cell_aliases.push_back(table_meta.field(i)->name());
    }
  }
}*/
static void wildcard_fields(Table *table, std::vector<Expression *> &expressions,
                            std::unordered_map<std::string, std::string> table_alias_reverse_map,
                            bool aliases_with_table_alias)
{
  const TableMeta &table_meta = table->table_meta();
  const int field_num = table_meta.field_num();
  for (int i = table_meta.sys_field_num(); i < field_num; i++) {
    std::string table_alias = table_alias_reverse_map.at(table_meta.name());
    std::string field_name = std::string(table_meta.field(i)->name());
    RelAttrSqlNode rel_attr;
    rel_attr.relation_alias = table_alias;
    rel_attr.attribute_name = field_name;
    FieldExpr *field_expr = new FieldExpr(rel_attr);
    if (aliases_with_table_alias) {
      field_expr->set_name(table_alias + "." + field_name);
    } else {
      field_expr->set_name(table_meta.field(i)->name());
    }
    expressions.push_back(field_expr);
  }
}

RC SelectStmt::create(Session *session, Db *db, const SelectSqlNode &select_sql, Stmt *&stmt,
                      const std::unordered_map<std::string, Table *> *merged_outer_queries_table_map,
                      const std::vector<ExprInfo> *merged_outer_queries_expr_infos)
{
  RC rc = RC::SUCCESS;

  if (nullptr == db) {
    LOG_WARN("invalid argument. db is null");
    return RC::INVALID_ARGUMENT;
  }

  // collect tables in `from` statement
  std::vector<Table *> tables;
  std::vector<Expression *> join_conditions;
  std::unordered_map<std::string, Table *> table_map;
  std::unordered_map<std::string, std::string> table_alias_map; // 不继承外层查询的，因为 operator 只负责本层查询的重命名
  std::unordered_map<std::string, std::string> table_alias_reverse_map;
  for (size_t i = 0; i < select_sql.relations.size(); i++) {
    const RelationSqlNode &relation = select_sql.relations[i];

    const char *table_name  = relation.relation_name.c_str();
    const char *table_alias = relation.relation_alias.c_str();
    if (nullptr == table_name) {
      LOG_WARN("invalid argument. relation name is null. index=%d", i);
      return RC::INVALID_ARGUMENT;
    }

    Table *table = db->find_table(table_name);
    if (nullptr == table) {
      LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }

    tables.push_back(table);
    table_map.insert(std::pair<std::string, Table *>(table_alias, table));
    table_alias_map.insert(std::pair<std::string, std::string>(table_alias, table_name));
    table_alias_reverse_map.insert(std::pair<std::string, std::string>(table_name, table_alias));

    // 处理表的连接条件
    if (relation.join_condition) {
      if (i == 0) {
        return RC::INTERNAL; // join condition 表示当前表与前一个表的连接条件，不会出现在最前面一项
      }
      std::unordered_map<std::string, Table *> join_table_map;
      join_table_map.insert(std::pair<std::string, Table *>(select_sql.relations[i - 1].relation_alias, tables[i - 1]));
      join_table_map.insert(std::pair<std::string, Table *>(relation.relation_alias, table));
      std::vector<ExprInfo> expr_infos_empty;
      rc = relation.join_condition->init_expr_tree(session, db, nullptr, table_map, expr_infos_empty);
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to init join condition expr tree");
        return rc;
      }
      if (relation.join_condition->has_aggre()) {
        return RC::SCHEMA_FIELD_MISSING;
      }

      if (!Value::can_convert(relation.join_condition->value_type(), BOOLEANS)) {
        LOG_WARN("join condition should be convertible to BOOLEAN");
        return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }
    }
    join_conditions.push_back(relation.join_condition);
  }

  Table *default_table = nullptr;
  if (tables.size() == 1) {
    default_table = tables[0];
    table_map.insert(std::pair<std::string, Table *>("", default_table));
    table_alias_map.insert(std::pair<std::string, std::string>("", default_table->name()));
  }

  // Collect expressions in `select` statement
  std::vector<Expression *> expressions;
  for (int i = static_cast<int>(select_sql.expressions.size()) - 1; i >= 0; i--) {
    Expression *expression = select_sql.expressions[i];

    // Process '*' attributes
    if (expression->type() == ExprType::FIELD)
    {
      FieldExpr *field_expr = static_cast<FieldExpr *>(expression);
      std::string rel_name = field_expr->table_name();
      std::string attr_name = field_expr->field_name();
      if (attr_name == "*"){
        if (rel_name == "*" || rel_name == ""){ // * or *.*(is *.* legal?)
          for (Table *table : tables) {
            wildcard_fields(table, expressions, table_alias_reverse_map, tables.size() > 1);
          }
        } else { // <table name>.*
          auto iter = table_map.find(rel_name);
          if (iter == table_map.end()) {
            LOG_WARN("no such table in FROM list: %s", rel_name);
            return RC::SCHEMA_FIELD_MISSING;
          }
          Table *table = iter->second;

          wildcard_fields(table, expressions, table_alias_reverse_map, tables.size() > 1);
        }

        // 判断有没有给 '*' 起别名
        if (rel_name == "" && field_expr->name() != "*" || rel_name != "" && field_expr->name() != rel_name + ".*") {
            LOG_WARN("'*' cannot have alias: %s", rel_name);
          return RC::SCHEMA_FIELD_MISSING;
        }
      } else {
        expressions.push_back(field_expr);
      }
    } else {
      expressions.push_back(expression);
    }
  }

  // 到这里为止，table_map 就开始并入外层查询的表了

  // FieldExpr 初始化查找字段时需要继承外层查询的的 table_map；对同一个键的修改覆盖原值
  if (merged_outer_queries_table_map) {
    for (const auto &pair : *merged_outer_queries_table_map) {
      // 逻辑上，内层对同一个键的修改覆盖外层的值；如果将外层的键值对并入内层，则等价于，内层已存在的键不并入
      if (table_map.count(pair.first) == 0) {
        table_map.insert(pair);
      }
    }
  }

  auto merged_expr_infos_outer = std::vector<ExprInfo>();
  if (merged_outer_queries_expr_infos) {
    for (ExprInfo expr_info : *merged_outer_queries_expr_infos) {
      merged_expr_infos_outer.push_back(expr_info);
    }
  }

  GroupsSqlNode groups = select_sql.groups;

  // if (groups.group_exprs.size() != 0) {
  //   // 有 group by 时
  //   // select 表达式中的非聚合函数只能使用 group by 表达式
  //   // having 表达式中的非聚合函数只能使用 select 表达式的别名和 group 表达式
  //   // 但先不管这个问题（懒。）
  //   // 把 select 表达式和 having 表达式中的非聚合函数套上 NONE，则可以等效为聚合函数
  //   for (auto &expression : expressions) {
  //     if (!expression->has_aggre()) {
  //       std::string name = expression->name();
  //       expression = new AggreExpr(AggreExpr::Type::NONE, expression);
  //       expression->set_name(name);
  //     }
  //   }
  //   if (groups.having) {
  //     if (!groups.having->has_aggre()) {
  //       groups.having = new AggreExpr(AggreExpr::Type::NONE, groups.having);
  //     }
  //   }
  // }

  // 初始化 select 表达式的表达式树
  // 递归地将表别名信息传到叶子节点的 FieldExpr 来初始化它们；初始化 AggreExpr 等；获取表达式是否含有聚合函数的信息
  bool exists_aggre = false; // 有聚合函数表达式
  bool exists_non_aggre = false; // 有非聚合函数表达式
  for (Expression *&expression : expressions) {
    rc = expression->init_expr_tree(session, db, &groups, table_map, merged_expr_infos_outer, true);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    if (expression->has_aggre()) {
      exists_aggre = true;
    } else {
      if (groups.group_exprs.size() != 0) {
        // 有 group by 时
        // select 表达式中的非聚合函数只能使用 group by 表达式
        // 但先不管这个问题 // TODO
        // 把 select 表达式中的非聚合函数套上 NONE，则可以等效为聚合函数
        std::string name = expression->name();
        expression = new AggreExpr(AggreExpr::Type::NONE, expression);
        expression->set_name("?" + name);
        expression->init_expr_tree(session, db, &groups, table_map, merged_expr_infos_outer, true);
        if (rc != RC::SUCCESS) {
          LOG_WARN("failed to init select expr tree after converting to aggre");
          return rc;
        }
        exists_aggre = true;
      } else {
        exists_non_aggre = true;
      }
    }
  }
  // 检查聚合函数的混用
  if (exists_aggre && exists_non_aggre) {
    LOG_WARN("Mixed use of aggregation and non-aggregation functions is illegal");
    return RC::SCHEMA_FIELD_MISSING;
  }
  // 接着，把表达式的一些信息字段存到 ExprInfo 结构体，以备使用
  std::vector<ExprInfo> expr_infos;
  for (Expression *expression : expressions) {
    expr_infos.push_back(ExprInfo(expression));
  }

  std::unique_ptr<Expression> condition(select_sql.condition);

  // 将本层查询表达式的信息和所有外层查询合并，用于 WHERE 条件和 ORDER BY 等表达式中 FieldExpr 的标识符查找
  // 上述去掉 WHERE，MySQL 不允许 WHERE 支持查询表达式的别名，所以得去掉这个功能了。。。
  auto merged_expr_infos = std::vector<ExprInfo>();
  for (ExprInfo expr_info : expr_infos) {
    merged_expr_infos.push_back(expr_info);
  }
  if (merged_outer_queries_expr_infos) {
    for (ExprInfo expr_info : *merged_outer_queries_expr_infos) {
      merged_expr_infos.push_back(expr_info);
    }
  }

  // 初始化 where 表达式的表达式树
  // 初始化 condition 表达式所有 FieldExpr 叶子节点（在 WHERE 中，它们既可以表示字段，也可以表示有别名的查询表达式）
  // 上述括号内的内容对 MySQL 来说不对，WHERE 不能支持查询表达式的别名，所以得去掉这个功能了。。。
  if (condition != nullptr) {
    rc = condition->init_expr_tree(session, db, nullptr, table_map, merged_expr_infos_outer, true);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    if (condition->has_aggre()) {
      LOG_WARN("aggregation function found in WHERE");
      return RC::SCHEMA_FIELD_MISSING;
    }
  }

  // 初始化 group by 表达式的表达式树
  for (const auto &group_expr : groups.group_exprs) {
    rc = group_expr->init_expr_tree(session, db, nullptr, table_map, merged_expr_infos, false);
    if (rc != RC::SUCCESS) {
      return rc;
    }
  }
  if (groups.having) {
    rc = groups.having->init_expr_tree(session, db, &groups, table_map, merged_expr_infos, false);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    // having 表达式中的非聚合函数只能使用 select 表达式的别名和 group 表达式
    // 但先不管这个问题 // TODO
    // 把 having 表达式中的非聚合函数套上 NONE，则可以等效为聚合函数
    if (!groups.having->has_aggre()) {
      groups.having = new AggreExpr(AggreExpr::Type::NONE, groups.having);
      rc = groups.having->init_expr_tree(session, db, &groups, table_map, merged_expr_infos_outer, true);
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to init having expr tree after converting to aggre");
        return rc;
      }
    }
  }

  // 初始化 order by 表达式的表达式树
  std::vector<OrderSqlNode> orders = select_sql.orders;
  for (const OrderSqlNode &order : orders) {
    rc = order.expression->init_expr_tree(session, db, nullptr, table_map, merged_expr_infos, false);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    if (order.expression->has_aggre()) {
      LOG_WARN("aggregation function found in ORDER BY");
      return RC::SCHEMA_FIELD_MISSING;
    }
  }

  // Copy expression names into string vector
  // Convert expressions into unique_ptr vector
  std::vector<std::string> expr_names;
  std::vector<std::unique_ptr<Expression>> exprs_unique;
  exprs_unique.reserve(expressions.size());
  for (auto *expression : expressions) {
    expr_names.push_back(expression->name());
    exprs_unique.push_back(std::unique_ptr<Expression>(expression));
  }

  LOG_INFO("got %d tables in from stmt and %d expressions in query stmt", tables.size(), expressions.size());

  // create filter statement in `where` statement
  FilterStmt *filter_stmt = nullptr;
  if (condition) {
    rc = FilterStmt::create(session, db, std::move(condition), filter_stmt);
    if (rc != RC::SUCCESS) {
      LOG_WARN("cannot construct filter stmt");
      return rc;
    }
  }

  // everything all right
  SelectStmt *select_stmt = new SelectStmt();
  select_stmt->expressions_.swap(exprs_unique);
  select_stmt->expr_infos_.swap(expr_infos);
  select_stmt->tables_.swap(tables);
  select_stmt->join_conditions_.swap(join_conditions);
  select_stmt->has_aggre_ = exists_aggre;
  select_stmt->table_alias_map_.swap(table_alias_map);
  select_stmt->filter_stmt_ = filter_stmt;
  select_stmt->groups_ = groups;
  select_stmt->orders_.swap(orders);
  stmt = select_stmt;
  return RC::SUCCESS;
}

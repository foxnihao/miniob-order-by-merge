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
// Created by Wangyunlai on 2021/5/14.
//

#pragma once

#include <memory>
#include <vector>
#include <string>

#include "common/log/log.h"
#include "sql/expr/tuple_cell.h"
#include "sql/parser/parse.h"
#include "sql/parser/value.h"
#include "sql/expr/expression.h"
#include "storage/record/record.h"

class Table;

/**
 * @defgroup Tuple
 * @brief Tuple 元组，表示一行数据，当前返回客户端时使用
 * @details 
 * tuple是一种可以嵌套的数据结构。
 * 比如select t1.a+t2.b from t1, t2;
 * 需要使用下面的结构表示：
 * @code {.cpp}
 *  Project(t1.a+t2.b)
 *        |
 *      Joined
 *      /     \
 *   Row(t1) Row(t2)
 * @endcode
 * 
 */

/**
 * @brief 元组的结构，包含哪些字段(这里成为Cell)，每个字段的说明
 * @ingroup Tuple
 */
class TupleSchema 
{
public:
  void append_cell(const TupleCellSpec &cell)
  {
    cells_.push_back(cell);
  }
  void append_cell(const char *table, const char *field)
  {
    append_cell(TupleCellSpec(table, field));
  }
  void append_cell(const char *alias)
  {
    append_cell(TupleCellSpec(alias));
  }
  int cell_num() const
  {
    return static_cast<int>(cells_.size());
  }
  const TupleCellSpec &cell_at(int i) const
  {
    return cells_[i];
  }

private:
  std::vector<TupleCellSpec> cells_;
};

/**
 * @brief 元组的抽象描述
 * @ingroup Tuple
 */
class Tuple 
{
public:
  Tuple() = default;
  virtual ~Tuple() = default;

  /**
   * @brief 获取元组中的Cell的个数
   * @details 个数应该与tuple_schema一致
   */
  virtual int cell_num() const = 0;

  /**
   * @brief 获取指定位置的Cell
   * 
   * @param index 位置
   * @param[out] cell  返回的Cell
   */
  virtual RC cell_at(int index, Value &cell) const = 0;

  /**
   * @brief 根据cell的描述，获取cell的值
   * 
   * @param spec cell的描述
   * @param[out] cell 返回的cell
   */
  virtual RC find_cell(const TupleCellSpec &spec, Value &cell) const = 0;

  virtual std::string to_string() const
  {
    std::string str;
    const int cell_num = this->cell_num();
    for (int i = 0; i < cell_num - 1; i++) {
      Value cell;
      cell_at(i, cell);
      str += cell.to_string();
      str += ", ";
    }

    if (cell_num > 0) {
      Value cell;
      cell_at(cell_num - 1, cell);
      str += cell.to_string();
    }
    return str;
  }
};

/**
 * @brief 一行数据的元组
 * @ingroup Tuple
 * @details 直接就是获取表中的一条记录
 */
class RowTuple : public Tuple 
{
public:
  RowTuple() = default;
  virtual ~RowTuple()
  {
    for (FieldExpr *spec : speces_) {
      delete spec;
    }
    speces_.clear();
  }

  RowTuple(const RowTuple &tuple)
  {
    this->record_ = new Record(*tuple.record_);
    this->table_ = tuple.table_;
    this->speces_.reserve(tuple.speces_.size());
    for (FieldExpr *spec : tuple.speces_) {
      this->speces_.push_back(new FieldExpr(*spec));
    }
  } 

  void set_record(Record *record)
  {
    this->record_ = record;
  }

  void set_schema(const Table *table, const std::vector<FieldMeta> *fields)
  {
    table_ = table;
    this->speces_.reserve(fields->size());
    for (const FieldMeta &field : *fields) {
      speces_.push_back(new FieldExpr(table, &field));
    }
  }

  int cell_num() const override
  {
    return speces_.size();
  }

  RC cell_at(int index, Value &cell) const override
  {
    if (index < 0 || index >= static_cast<int>(speces_.size())) {
      LOG_WARN("invalid argument. index=%d", index);
      return RC::INVALID_ARGUMENT;
    }

    FieldExpr *field_expr = speces_[index];
    const FieldMeta *field_meta = field_expr->field().meta();
    cell.set_type(field_meta->type());
    cell.set_data(this->record_->data() + field_meta->offset(), field_meta->len());
    return RC::SUCCESS;
  }

  RC find_cell(const TupleCellSpec &spec, Value &cell) const override
  {
    const char *table_name = spec.table_name();
    const char *field_name = spec.field_name();
    if (0 != strcmp(table_name, table_->name())) {
      return RC::NOTFOUND;
    }

    for (size_t i = 0; i < speces_.size(); ++i) {
      const FieldExpr *field_expr = speces_[i];
      const Field &field = field_expr->field();
      if (0 == strcmp(field_name, field.field_name())) {
        return cell_at(i, cell);
      }
    }
    return RC::NOTFOUND;
  }

#if 0
  RC cell_spec_at(int index, const TupleCellSpec *&spec) const override
  {
    if (index < 0 || index >= static_cast<int>(speces_.size())) {
      LOG_WARN("invalid argument. index=%d", index);
      return RC::INVALID_ARGUMENT;
    }
    spec = speces_[index];
    return RC::SUCCESS;
  }
#endif

  Record &record()
  {
    return *record_;
  }

  const Record &record() const
  {
    return *record_;
  }

private:
  Record *record_ = nullptr;
  const Table *table_ = nullptr;
  std::vector<FieldExpr *> speces_;
};

// 将数据库记录元组中的表原名与表别名建立映射
class TableAliasTuple : public Tuple
{
public:
  TableAliasTuple(const std::unordered_map<std::string, std::string> &table_alias_map)
    : table_alias_map_(table_alias_map)
  {
    // 性能优化的预处理
    no_alias_ = true;
    if (table_alias_map.size() == 0) {
      no_alias_ = true;
    } else {
      for (auto &pair : table_alias_map) {
        auto alias = pair.first;
        auto name = pair.second;
        if (alias == "") {
          default_table_name_ = name; // 找到默认表名
        } else if (alias != name) {
          no_alias_ = false; // 找到起了别名的表
        }
      }
    }
  }
  virtual ~TableAliasTuple() {}

  void set_tuple(Tuple *tuple) { tuple_ = tuple; }
  int cell_num() const override { return tuple_->cell_num(); }
  RC cell_at(int index, Value &cell) const override { return tuple_->cell_at(index, cell); }
  RC find_cell(const TupleCellSpec &spec, Value &cell) const override
  {
    std::string table_alias = std::string(spec.table_name());
    std::string field_name = std::string(spec.field_name());
    std::string table_name;

    // 性能优化：尽量不用 map
    if (table_alias == "" && default_table_name_ != "") { // 遇到空，直接找 default_table_name_
      table_name = default_table_name_;
    } else if (no_alias_) { // 没有起过别名，就可以跳过 table alias 这一层了
      return tuple_->find_cell(spec, cell);
    } else {
      int count = table_alias_map_.count(table_alias);
      if (count == 0) return RC::NOTFOUND;
      if (count > 1) return RC::INTERNAL; // 参考“ambiguous column name”报错
      table_name = table_alias_map_.at(table_alias);
    }

    return tuple_->find_cell(TupleCellSpec(table_name.c_str(), field_name.c_str()), cell);
  }

  Tuple *tuple() const { return tuple_; }

private:
  // 映射关系
  const std::unordered_map<std::string, std::string> &table_alias_map_;

  // 子元组（应为数据库记录元组）
  Tuple *tuple_ = nullptr;

  // 性能优化
  bool no_alias_ = false; // true 表示查询表达式没有给表别名
  std::string default_table_name_ = ""; // 默认表名：如果查询表达式只有一个表，则它就是默认表
};

class ExpressionTuple : public Tuple 
{
public:
  ExpressionTuple(std::vector<std::unique_ptr<Expression>> &&expressions)
  {
    expressions_.swap(expressions);
  }

  ExpressionTuple(const ExpressionTuple &other) = delete;
  
  virtual ~ExpressionTuple()
  {
  }

  int cell_num() const override
  {
    return expressions_.size();
  }

  RC cell_at(int index, Value &cell) const override
  {
    if (index < 0 || index >= static_cast<int>(expressions_.size())) {
      return RC::INTERNAL;
    }

    const Expression *expr = expressions_[index].get();
    return expr->try_get_value(cell);
  }

  RC cell_at(int index, Value &cell, Expression *&expr) const
  {
    if (index < 0 || index >= static_cast<int>(expressions_.size())) {
      return RC::INTERNAL;
    }

    expr = expressions_[index].get();
    return expr->try_get_value(cell);
  }

  RC cell_at(int index, const Tuple *tuple, Value &cell) const
  {
    if (index < 0 || index >= static_cast<int>(expressions_.size())) {
      return RC::INTERNAL;
    }

    Expression *expr = expressions_[index].get();
    return expr->get_value(*tuple, cell);
  }

  RC find_cell(const TupleCellSpec &spec, Value &cell) const override
  {
    for (const std::unique_ptr<Expression> &expr : expressions_) {
      if (0 == strcmp(spec.alias(), expr->name().c_str())) {
        return expr->try_get_value(cell);
      }
    }
    return RC::NOTFOUND;
  }

  RC find_cell(const TupleCellSpec &spec, const Tuple *tuple, Value &cell) const
  {
    for (const std::unique_ptr<Expression> &expr : expressions_) {
      if (0 == strcmp(spec.alias(), expr->name().c_str())) {
        return expr->get_value(*tuple, cell);
      }
    }
    return RC::NOTFOUND;
  }

  const std::vector<std::unique_ptr<Expression>> &expressions() const
  {
    return expressions_;
  }

private:
  std::vector<std::unique_ptr<Expression>> expressions_;
};

/**
 * @brief 将两个tuple合并为一个tuple；右tuple可以为空
 * @ingroup Tuple
 * @details 在join算子中使用
 */
class JoinedTuple : public Tuple 
{
public:
  JoinedTuple() = default;
  virtual ~JoinedTuple() = default;

  void set_left(const Tuple *left)
  {
    left_ = left;
  }
  void set_right(const Tuple *right)
  {
    right_ = right;
  }

  int cell_num() const override
  {
    return left_->cell_num() + (right_ ? right_->cell_num() : 0);
  }

  RC cell_at(int index, Value &value) const override
  {
    const int left_cell_num = left_->cell_num();
    if (index > 0 && index < left_cell_num) {
      return left_->cell_at(index, value);
    }

    if (right_) {
      if (index >= left_cell_num && index < left_cell_num + right_->cell_num()) {
        return right_->cell_at(index - left_cell_num, value);
      }
    }

    return RC::NOTFOUND;
  }

  RC find_cell(const TupleCellSpec &spec, Value &value) const override
  {
    RC rc = left_->find_cell(spec, value);
    if (rc == RC::SUCCESS || rc != RC::NOTFOUND) {
      return rc;
    }
    if (right_) {
      return right_->find_cell(spec, value);
    }
    return rc;
  }

  const Tuple *const &left() const { return left_; }
  const Tuple *const &right() const { return right_; }

private:
  const Tuple *left_ = nullptr;
  const Tuple *right_ = nullptr;
};

/**
 * @brief 从一行数据中，选择部分字段组成的元组，也就是投影操作
 * @ingroup Tuple
 * @details 一般在select语句中使用。
 * 投影也可以是很复杂的操作，比如某些字段需要做类型转换、重命名、表达式运算、函数计算等。
 * 当前的实现是比较简单的，只是选择部分字段，不做任何其他操作。
 * 上面这句现在已经可以去掉了；当前 ProjectTuple 用于将数据投影到一组有别名的表达式上，
 * 并“隐式”保留投影前的数据（不算在下标中，但 find_cell 可以找到），用于更上层的第二次 Predicate 的表达式求值
 */
class ProjectTuple : public Tuple 
{
public:
  ProjectTuple() = default;
  ProjectTuple(std::vector<std::unique_ptr<Expression>> &&expressions, const Tuple *outer_query_tuple)
      : query_tuple_(std::move(expressions))
  {
    src_tuple_.set_right(outer_query_tuple);
  }
  virtual ~ProjectTuple()
  {}

  void set_tuple(Tuple *tuple)
  {
    src_tuple_.set_left(tuple);
  }

  int cell_num() const override
  {
    return query_tuple_.cell_num();
  }

  // 用 Expression 的 get_value，需要 tuple
  RC cell_at(int index, Value &cell) const override
  {
    return query_tuple_.cell_at(index, &src_tuple_, cell);
  }

  // 用 Expression 的 try_get_value，不需要 tuple
  RC try_cell_at(int index, Value &cell) const
  {
    return query_tuple_.cell_at(index, cell);
  }
  RC try_cell_at(int index, Value &cell, Expression *&expr) const
  {
    return query_tuple_.cell_at(index, cell, expr);
  }

  RC find_cell(const TupleCellSpec &spec, Value &cell) const override
  {
    // 从查询表达式元组中找时，用到的是 spec 中的别名
    // 从数据记录元组中找时，用到的是 spec 中的表名和字段名

    RC rc = RC::SUCCESS;
    if (src_tuple_.left() == nullptr) {
      rc = query_tuple_.find_cell(spec, cell); // already EOF, then try_get_value, get results of group-by
    } else {
      rc = query_tuple_.find_cell(spec, &src_tuple_, cell); // not yet EOF, get_value
    }
    if (rc == RC::SUCCESS || rc != RC::NOTFOUND) return rc;

    return src_tuple_.find_cell(spec, cell);
  }

private:
  const ExpressionTuple query_tuple_; // 输出的元组
  //Tuple *tuple_ = nullptr; // 从下层 operator 传来的表查询结果元组
  //Tuple *outer_query_tuple_ = nullptr; // 外层查询元组，是子查询的数据源的一部分
  JoinedTuple src_tuple_; // 为 tuple_ 和 outer_query_tuple_ 的联合，并且 JoinedTuple.find_cell 先找左再找右
};

/**
 * @brief 一些常量值组成的Tuple
 * @ingroup Tuple
 */
class ValueListTuple : public Tuple 
{
public:
  ValueListTuple() = default;
  virtual ~ValueListTuple() = default;

  void set_cells(const std::vector<Value> &cells)
  {
    cells_ = cells;
  }

  void set_cell_names(const std::vector<std::string> &cell_names)
  {
    for (int i = 0; i < cell_names.size(); ++i) {
      cell_name_map_.insert(std::pair<std::string, int>(cell_names[i], i));
    }
  }

  virtual int cell_num() const override
  {
    return static_cast<int>(cells_.size());
  }

  virtual RC cell_at(int index, Value &cell) const override
  {
    if (index < 0 || index >= cell_num()) {
      return RC::NOTFOUND;
    }

    cell = cells_[index];
    return RC::SUCCESS;
  }

  virtual RC find_cell(const TupleCellSpec &spec, Value &cell) const override
  {
    if (cell_name_map_.count(spec.alias()) == 0) return RC::NOTFOUND;
    cell = cells_[cell_name_map_.at(spec.alias())];
    return RC::SUCCESS;
  }

private:
  std::vector<Value> cells_;
  std::unordered_map<std::string, int> cell_name_map_;
};

class OrderByTuple : public Tuple
{
  public:
  OrderByTuple() = default;
  OrderByTuple(const ValueListTuple cell_values_tuple, const std::vector<Value> &order_key_values)
    : cell_values_tuple_(cell_values_tuple), order_key_values_(order_key_values)
  {}
  virtual ~OrderByTuple() = default;

  virtual int cell_num() const override
  {
    return cell_values_tuple_.cell_num();
  }

  virtual RC cell_at(int index, Value &cell) const override
  {
    return cell_values_tuple_.cell_at(index, cell);
  }

  virtual RC find_cell(const TupleCellSpec &spec, Value &cell) const override
  {
    return cell_values_tuple_.find_cell(spec, cell);
  }

  const ValueListTuple &cell_values_tuple() const
  {
    return cell_values_tuple_;
  }

  const std::vector<Value> &order_key_values() const
  {
    return order_key_values_;
  }

private:
  ValueListTuple cell_values_tuple_; // 一行查询结果的所有列值
  std::vector<Value> order_key_values_; // 该行对应的所有 order by expression 的值
};
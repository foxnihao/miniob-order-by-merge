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
// Created by Wangyunlai on 2022/07/05.
//

#pragma once

#include <string.h>
#include <memory>
#include <string>
#include <cmath>
#include <ctime>
#include <vector>

#include "storage/field/field.h"
#include "sql/parser/value.h"
#include "common/log/log.h"

class Tuple;
class Stmt;
class PhysicalOperator;
class Session;
class Db;

/**
 * @defgroup Expression
 * @brief 表达式
 */

/**
 * @brief 表达式类型
 * @ingroup Expression
 */
enum class ExprType 
{
  NONE,
  STAR,         ///< 星号，表示所有字段
  FIELD,        ///< 字段。在实际执行时，根据行数据内容提取对应字段的值
  VALUE,        ///< 常量值
  CAST,         ///< 需要做类型转换的表达式
  COMPARISON,   ///< 需要做比较的表达式
  CONJUNCTION,  ///< 多个表达式使用同一种关系(AND或OR)来联结
  ARITHMETIC,   ///< 算术运算
  FUNCTION,     ///< 函数表达式
  SUBQUERY,     ///< 子查询表达式
  AGGRE,        ///< 聚合函数表达式
};

struct ExprInfo
{
public:
  std::string name = "";
  std::string short_name = ""; // 如果只是一个 FieldExpr，那么这个是它不带表名的字段名，用于 create-table-select
  AttrType value_type = AttrType::UNDEFINED;
  bool inferred_nullable = default_inferred_nullable;
  size_t inferred_length = default_inferred_length;

  static const bool default_inferred_nullable = true;
  static const size_t default_inferred_length = 4;

  ExprInfo(Expression *&expression);
};

/**
 * @brief 表达式的抽象描述
 * @ingroup Expression
 * @details 在SQL的元素中，任何需要得出值的元素都可以使用表达式来描述
 * 比如获取某个字段的值、比较运算、类型转换
 * 当然还有一些当前没有实现的表达式，比如算术运算。
 *
 * 通常表达式的值，是在真实的算子运算过程中，拿到具体的tuple后
 * 才能计算出来真实的值。但是有些表达式可能就表示某一个固定的
 * 值，比如ValueExpr。
 */
class Expression 
{
public:
  Expression() = default;
  virtual ~Expression() = default;

  /**
   * @brief 根据具体的tuple，来计算当前表达式的值。tuple有可能是一个具体某个表的行数据
   */
  virtual RC get_value(const Tuple &tuple, Value &value) const = 0;

  /**
   * @brief 在 Stmt 的创建阶段，下放一些查询信息，用来：
   *        初始化叶子节点 FieldExpr 的值类型，并判断其对应的字段或表达式的存在性；
   *        生成子查询表达式的执行计划。
   *        （参数中的表达式组如果为空，则说明 FieldExpr 的标识符不表示任何查询表达式的别名）
   *        （如果 failure_when_found_expr 为 true，则在 merged_expr_infos 中找到时会返回错误，
   *          用以应对子查询不能使用外部查询中别名的问题。。。）
   */
  virtual RC init_expr_tree(Session *session, Db *db, const GroupsSqlNode *groups,
                            const std::unordered_map<std::string, Table *> &merged_table_map,
                            const std::vector<ExprInfo> &merged_expr_infos,
                            bool failure_when_found_expr = false)
  { return RC::UNIMPLENMENT; }

  /**
   * @brief 在没有实际运行的情况下，也就是无法获取tuple的情况下，尝试获取表达式的值
   * @details 有些表达式的值是固定的，比如ValueExpr，这种情况下可以直接获取值
   */
  virtual RC try_get_value(Value &value) const
  {
    return RC::UNIMPLENMENT;
  }

  /**
   * @brief 表达式的类型
   * 可以根据表达式类型来转换为具体的子类
   */
  virtual ExprType type() const = 0;

  /**
   * @brief 表达式值的类型
   * @details 一个表达式运算出结果后，只有一个值
   */
  virtual AttrType value_type() const = 0;

  /**
   * @brief 表达式的名字，比如是字段名称，或者用户在执行SQL语句时输入的内容
   */
  virtual std::string name() const { return name_; }
  virtual void set_name(std::string name) { name_ = name; }

  bool inferred_nullable() const { return inferred_nullable_; }
  size_t inferred_length() const { return inferred_length_; }

  bool has_aggre() const { return has_aggre_; }

private:
  std::string  name_;

protected:
  bool inferred_nullable_ = true;
  size_t inferred_length_ = 0;
  bool has_aggre_ = false;
};

/**
 * @brief 字段表达式
 * @ingroup Expression
 * @details 目前被我分为了两种：Field 型、RelAttrSqlNode 型。
 *          前者的残余用法在 field_ 非空时保留，
 *          后者则可以表示带别名的表的字段（用 rel_attr 结构标识）或带别名的查询表达式（用基类的 name 标识）。
 */
class FieldExpr : public Expression
{
public:
  FieldExpr() = default;
  FieldExpr(RelAttrSqlNode rel_attr) : rel_attr_(rel_attr)
  {}
  FieldExpr(const Table *table, const FieldMeta *field) : field_(table, field)
  {}
  FieldExpr(const Field &field) : field_(field)
  {}

  virtual ~FieldExpr() = default;

  ExprType type() const override { return ExprType::FIELD; }
  AttrType value_type() const override {
    if (field_.meta() != nullptr) return field_.attr_type();
    else return attr_type_;
  }

  Field &field() { return field_; }

  const Field &field() const { return field_; }

  const char *table_name() const {
    if (field_.meta() != nullptr) return field_.table_name();
    else return rel_attr_.relation_alias.c_str();
  }

  const char *field_name() const {
    if (field_.meta() != nullptr) return field_.field_name();
    else return rel_attr_.attribute_name.c_str();
  }

  RC get_value(const Tuple &tuple, Value &value) const override;
  RC init_expr_tree(Session *session, Db *db, const GroupsSqlNode *groups, const std::unordered_map<std::string, Table *> &merged_table_map, const std::vector<ExprInfo> &merged_expr_infos, bool failure_when_found_expr = true) override;

private:
  RelAttrSqlNode rel_attr_;
  AttrType attr_type_ = AttrType::UNDEFINED;
  Field field_;
};

/**
 * @brief 常量值表达式
 * @ingroup Expression
 */
class ValueExpr : public Expression 
{
public:
  ValueExpr() = default;
  explicit ValueExpr(const Value &value) : value_(value)
  {}

  virtual ~ValueExpr() = default;

  RC get_value(const Tuple &tuple, Value &value) const override;
  RC init_expr_tree(Session *session, Db *db, const GroupsSqlNode *groups, const std::unordered_map<std::string, Table *> &merged_table_map, const std::vector<ExprInfo> &merged_expr_infos, bool failure_when_found_expr = true) override;
  RC try_get_value(Value &value) const override { value = value_; return RC::SUCCESS; }

  ExprType type() const override { return ExprType::VALUE; }

  AttrType value_type() const override { return value_.attr_type(); }

  void get_value(Value &value) const { value = value_; }

  const Value &get_value() const { return value_; }

private:
  Value value_;
};

/**
 * @brief 类型转换表达式
 * @ingroup Expression
 */
class CastExpr : public Expression 
{
public:
  CastExpr(std::unique_ptr<Expression> child, AttrType cast_type);
  virtual ~CastExpr();

  ExprType type() const override
  {
    return ExprType::CAST;
  }
  RC get_value(const Tuple &tuple, Value &value) const override;
  RC init_expr_tree(Session *session, Db *db, const GroupsSqlNode *groups, const std::unordered_map<std::string, Table *> &merged_table_map, const std::vector<ExprInfo> &merged_expr_infos, bool failure_when_found_expr = true) override;

  RC try_get_value(Value &value) const override;

  AttrType value_type() const override { return cast_type_; }

  std::unique_ptr<Expression> &child() { return child_; }

private:
  RC cast(const Value &value, Value &cast_value) const;

private:
  std::unique_ptr<Expression> child_;  ///< 从这个表达式转换
  AttrType cast_type_;  ///< 想要转换成这个类型
};

/**
 * @brief 比较表达式
 * @ingroup Expression
 */
class ComparisonExpr : public Expression 
{
public:
  ComparisonExpr(CompOp comp, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right);
  ComparisonExpr(CompOp comp, Expression *left, Expression *right);
  virtual ~ComparisonExpr();

  ExprType type() const override { return ExprType::COMPARISON; }

  RC get_value(const Tuple &tuple, Value &value) const override;
  RC init_expr_tree(Session *session, Db *db, const GroupsSqlNode *groups, const std::unordered_map<std::string, Table *> &merged_table_map, const std::vector<ExprInfo> &merged_expr_infos, bool failure_when_found_expr = true) override;

  AttrType value_type() const override { return BOOLEANS; }

  CompOp comp() const { return comp_; }

  std::unique_ptr<Expression> &left()  { return left_;  }
  std::unique_ptr<Expression> &right() { return right_; }

  /**
   * 尝试在没有tuple的情况下获取当前表达式的值
   * 在优化的时候，可能会使用到
   */
  RC try_get_value(Value &value) const override;

  /**
   * compare the two tuple cells
   */
  RC compare_value(const Value &left, const Value &right, Value &result) const;

private:
  CompOp comp_;
  std::unique_ptr<Expression> left_;
  std::unique_ptr<Expression> right_;
};

/**
 * @brief 联结表达式
 * @ingroup Expression
 * 多个表达式使用同一种关系(AND或OR)来联结
 * 当前miniob仅有AND操作
 */
class ConjunctionExpr : public Expression 
{
public:
  enum class Type {
    AND,
    OR,
  };

public:
  ConjunctionExpr(Type type, std::vector<std::unique_ptr<Expression>> &children);
  ConjunctionExpr(Type type, Expression *left, Expression *right);
  virtual ~ConjunctionExpr() = default;

  ExprType type() const override { return ExprType::CONJUNCTION; }

  AttrType value_type() const override { return BOOLEANS; }

  RC get_value(const Tuple &tuple, Value &value) const override;
  RC init_expr_tree(Session *session, Db *db, const GroupsSqlNode *groups, const std::unordered_map<std::string, Table *> &merged_table_map, const std::vector<ExprInfo> &merged_expr_infos, bool failure_when_found_expr = true) override;

  Type conjunction_type() const { return conjunction_type_; }

  std::vector<std::unique_ptr<Expression>> &children() { return children_; }

private:
  Type conjunction_type_;
  std::vector<std::unique_ptr<Expression>> children_;
};

/**
 * @brief 算术表达式
 * @ingroup Expression
 */
class ArithmeticExpr : public Expression 
{
public:
  enum class Type {
    ADD,
    SUB,
    MUL,
    DIV,
    NEGATIVE,
  };

public:
  ArithmeticExpr(Type type, Expression *left, Expression *right);
  ArithmeticExpr(Type type, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right);
  virtual ~ArithmeticExpr() = default;

  ExprType type() const override { return ExprType::ARITHMETIC; }

  AttrType value_type() const override;

  RC get_value(const Tuple &tuple, Value &value) const override;
  RC init_expr_tree(Session *session, Db *db, const GroupsSqlNode *groups, const std::unordered_map<std::string, Table *> &merged_table_map, const std::vector<ExprInfo> &merged_expr_infos, bool failure_when_found_expr = true) override;
  RC try_get_value(Value &value) const override;

  Type arithmetic_type() const { return arithmetic_type_; }

  std::unique_ptr<Expression> &left() { return left_; }
  std::unique_ptr<Expression> &right() { return right_; }

private:
  RC calc_value(const Value &left_value, const Value &right_value, Value &value) const;
  
private:
  Type arithmetic_type_;
  std::unique_ptr<Expression> left_;
  std::unique_ptr<Expression> right_;
};

class FunctionExpr : public Expression {
public:
  enum class Type {
    LENGTH,
    ROUND,
    DATE_FORMAT,
    NOT,
  };

  FunctionExpr(Type type, std::vector<Expression *> args);
  FunctionExpr(Type type, std::vector<std::unique_ptr<Expression>> &args);

  virtual ~FunctionExpr() {}

  ExprType type() const override { return ExprType::FUNCTION; }

  Type function_type() const { return function_type_; }

  AttrType value_type() const;

  RC get_value(const Tuple &tuple, Value &value) const override;
  RC init_expr_tree(Session *session, Db *db, const GroupsSqlNode *groups, const std::unordered_map<std::string, Table *> &merged_table_map, const std::vector<ExprInfo> &merged_expr_infos, bool failure_when_found_expr = true) override;
  RC try_get_value(Value &value) const override;

private:
  Type function_type_;
  std::vector<std::unique_ptr<Expression>> args_;

  RC eval_value(const Value &arg0_value, const Value &arg1_value, Value &value) const;
  RC eval_length(const Value &arg_value, Value &value) const;
  RC eval_round(const Value &arg0_value, const Value &arg1_value, Value &value) const;
  RC eval_date_format(const Value &arg0_value, const Value &arg1_value, Value &value) const;
  RC eval_not(const Value &arg_value, Value &value) const;
};

class SubqueryExpr : public Expression {
public:
  SubqueryExpr (std::string sql) : sql_(sql) {}

  virtual ~SubqueryExpr() = default;

  RC get_value(const Tuple &tuple, Value &value) const override;
  RC init_expr_tree(Session *session, Db *db, const GroupsSqlNode *groups, const std::unordered_map<std::string, Table *> &merged_table_map, const std::vector<ExprInfo> &merged_expr_infos, bool failure_when_found_expr = true) override;
  RC try_get_value(Value &value) const override;

  ExprType type() const override { return ExprType::SUBQUERY; }

  AttrType value_type() const override { return AttrType::VAL_LIST; }

  const std::string &sql() const
  {
    return sql_;
  }

  const std::vector<ExprInfo> &expr_infos() const
  {
    return expr_infos_;
  }

private:
  RC generate_oper(const Tuple *outer_query_tuple, std::unique_ptr<PhysicalOperator> &oper) const;
  RC execute_query(std::unique_ptr<PhysicalOperator> &oper, std::vector<Value> &query_result) const;

private:
  std::string sql_ = "";
  Session *session_ = nullptr;
  Db *db_ = nullptr;
  std::unordered_map<std::string, Table *> merged_table_map_;
  std::vector<ExprInfo> merged_expr_infos_;
  mutable std::vector<ExprInfo> expr_infos_;
};

bool LIKE_Match(const char *left_value, const char *like_expr);

struct AggreResult
{
public:
  int cnt = 0;
  float sum = 0;
  Value max_val = Value::null_value();
  Value min_val = Value::null_value();
  Value first_val = Value::null_value();
};

class AggreExpr : public Expression
{
public:
  enum class Type {
    CNT,
    SUM,
    AVG,
    MAX,
    MIN,
    NONE,
  };
  
  struct GroupHash {
    size_t operator() (const std::vector<Value>& vec) const{
      std::size_t seed = vec.size();
      for (const auto& value : vec) {
        auto x = std::hash<std::string>()(attr_type_to_string(value.attr_type()) + value.to_string());
        x = ((x >> 16) ^ x) * 0x45d9f3b;
        x = ((x >> 16) ^ x) * 0x45d9f3b;
        x = (x >> 16) ^ x;
        seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
      }
      return seed;
    }
  };
  struct GroupEqual {
    bool operator() (const std::vector<Value>& lhs, const std::vector<Value>& rhs) const{
      if (lhs.size() != rhs.size()) {
        return false;
      }
      for (int i = 0; i < lhs.size(); ++i) {
        const Value &lval = lhs[i], &rval = rhs[i];
        if (lval.is_null() && rval.is_null()) {
          return true;
        }
        if (lval.is_null() ^ rval.is_null()) {
          return false;
        }
        Value cmp_result;
        if (lval.compare(rval, cmp_result) != RC::SUCCESS) {
          return false;
        }
        if (cmp_result.is_null()) {
          return true;
        }
        if (cmp_result.get_int() != 0) {
          return false;
        }
      }
      return true;
    }
  };

  AggreExpr(Type type, Expression *arg)
    : aggre_type_(type)
  {
    args_.push_back(std::unique_ptr<Expression>(arg));
  }

  AggreExpr(Type type, std::vector<Expression *> &args)
    : aggre_type_(type)
  {
    args_.reserve(args.size());
    for (auto* expr : args) {
      std::unique_ptr<Expression> uni_expr(expr);
      args_.push_back(std::move(uni_expr));
    }
  }

  AggreExpr(Type type, std::vector<std::unique_ptr<Expression>> &args)
    : aggre_type_(type), args_(std::move(args))
  {}

  virtual ~AggreExpr() {}

  ExprType type() const override { return ExprType::AGGRE; }

  Type aggre_type() const { return aggre_type_; }

  AttrType value_type() const
  {
    switch (aggre_type_) {
      case Type::CNT:
        return INTS;
      case Type::SUM:
      case Type::AVG:
        return FLOATS;
      case Type::MAX:
      case Type::MIN:
      case Type::NONE:
        if (args_.size() != 0) return args_[0]->value_type();
        else return UNDEFINED;
      default:
        return UNDEFINED;
    }
  }

  RC get_value(const Tuple &tuple, Value &value) const override;
  RC init_expr_tree(Session *session, Db *db, const GroupsSqlNode *groups, const std::unordered_map<std::string, Table *> &merged_table_map, const std::vector<ExprInfo> &merged_expr_infos, bool failure_when_found_expr = true) override;
  RC try_get_value(Value &value) const override;

private:
  Type aggre_type_;
  std::vector<std::unique_ptr<Expression>> args_;
  GroupsSqlNode groups_;
  mutable std::unordered_map<std::vector<Value>, AggreResult, GroupHash, GroupEqual> aggre_results_;
  mutable int group_index_ = 0;
};
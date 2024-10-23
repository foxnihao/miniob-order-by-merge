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

#include <regex>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <unordered_set>
#include "sql/expr/expression.h"
#include "sql/expr/tuple.h"
#include "sql/stmt/select_stmt.h"
#include "common/lang/string.h"
#include "sql/operator/logical_operator.h"
#include "sql/operator/physical_operator.h"
#include "sql/optimizer/logical_plan_generator.h"
#include "sql/optimizer/physical_plan_generator.h"
#include "sql/optimizer/rewriter.h"
#include "session/session.h"
#include "event/session_event.h"

using namespace std;

ExprInfo::ExprInfo(Expression *&expression)
{
  name = expression->name();
  if (expression->type() == ExprType::FIELD) {
    short_name = static_cast<FieldExpr *>(expression)->field_name();
  } else {
    short_name = name;
  }
  value_type = expression->value_type();
  inferred_nullable = expression->inferred_nullable();
  inferred_length = expression->inferred_length();
}

RC FieldExpr::get_value(const Tuple &tuple, Value &value) const
{
  return tuple.find_cell(TupleCellSpec(table_name(), field_name(), this->name().c_str()), value);
}

RC FieldExpr::init_expr_tree(Session *session, Db *db, const GroupsSqlNode *groups,
                                  const std::unordered_map<std::string, Table *> &merged_table_map,
                                  const std::vector<ExprInfo> &merged_expr_infos, bool failure_when_found_expr)
{
  if (attr_type_ != AttrType::UNDEFINED) return RC::SUCCESS;

  has_aggre_ = false;

  // 查找表达式的顺序应该是按嵌套查询由内而外的顺序
  for (ExprInfo expr_info: merged_expr_infos) {
    if (this->name() == expr_info.name) {
      attr_type_ = expr_info.value_type;
      inferred_nullable_ = expr_info.inferred_nullable;
      inferred_length_ = expr_info.inferred_length;
      if (failure_when_found_expr) {
        LOG_WARN("reference to expression alias %s is unsupported", this->name().c_str());
        return RC::NOTFOUND;
      }
      return failure_when_found_expr ? RC::NOTFOUND : RC::SUCCESS;
    }
  }
  // merged_table_map 应该由 FieldExpr 所在的所有嵌套查询的 table_map 合并而成
  // merged_table_map 的键是唯一的，在构建过程中内层的键应该覆盖外层的
  if (merged_table_map.count(table_name()) != 0) {
    Table *table = merged_table_map.at(table_name());
    const TableMeta &table_meta = table->table_meta();
    const int field_num = table_meta.field_num();
    for (int i = table_meta.sys_field_num(); i < field_num; i++) {
      const FieldMeta *table_field_meta = table_meta.field(i);
      std::string table_field_name = std::string(table_field_meta->name());
      if (field_name() == table_field_name) {
        attr_type_ = table_field_meta->type();
        inferred_nullable_ = table_field_meta->nullable();
        inferred_length_ = table_field_meta->len();
        return RC::SUCCESS;
      }
    }
  }
  LOG_WARN("FieldExpr %s has failed to find its reference", this->name().c_str());
  return RC::NOTFOUND;
}

RC ValueExpr::get_value(const Tuple &tuple, Value &value) const
{
  value = value_;
  return RC::SUCCESS;
}

RC ValueExpr::init_expr_tree(Session *session, Db *db, const GroupsSqlNode *groups, const std::unordered_map<std::string, Table *> &merged_table_map, const std::vector<ExprInfo> &merged_expr_infos, bool failure_when_found_expr)
{
  if (value_.attr_type() == DATES) {
    int date = *(int *)value_.data();
    bool is_legal = check_date(date/10000,(date/100)%100,date%100);
    if(!is_legal)  return RC::SCHEMA_FIELD_MISSING;
  }

  inferred_nullable_ = value_.is_null();
  inferred_length_ = value_.length();

  has_aggre_ = false;

  return RC::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////
CastExpr::CastExpr(unique_ptr<Expression> child, AttrType cast_type)
    : child_(std::move(child)), cast_type_(cast_type)
{}

CastExpr::~CastExpr()
{}

RC CastExpr::cast(const Value &value, Value &cast_value) const
{
  RC rc = RC::SUCCESS;
  if (this->value_type() == value.attr_type()) {
    cast_value = value;
    return rc;
  }

  switch (cast_type_) {
    case BOOLEANS: {
      bool val = value.get_boolean();
      cast_value.set_boolean(val);
    } break;
    default: {
      rc = RC::INTERNAL;
      LOG_WARN("unsupported convert from type %d to %d", child_->value_type(), cast_type_);
    }
  }
  return rc;
}

RC CastExpr::get_value(const Tuple &tuple, Value &cell) const
{
  RC rc = child_->get_value(tuple, cell);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  return cast(cell, cell);
}

RC CastExpr::init_expr_tree(Session *session, Db *db, const GroupsSqlNode *groups, const std::unordered_map<std::string, Table *> &merged_table_map, const std::vector<ExprInfo> &merged_expr_infos, bool failure_when_found_expr)
{
  RC rc = child_->init_expr_tree(session, db, groups, merged_table_map, merged_expr_infos, failure_when_found_expr);
  if (rc != RC::SUCCESS) return rc;

  inferred_nullable_ = child_->inferred_nullable();
  inferred_length_ = child_->inferred_length();

  has_aggre_ = child_->has_aggre();

  return rc;
}

RC CastExpr::try_get_value(Value &value) const
{
  RC rc = child_->try_get_value(value);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  return cast(value, value);
}

////////////////////////////////////////////////////////////////////////////////

ComparisonExpr::ComparisonExpr(CompOp comp, unique_ptr<Expression> left, unique_ptr<Expression> right)
    : comp_(comp), left_(std::move(left)), right_(std::move(right))
{}
ComparisonExpr::ComparisonExpr(CompOp comp, Expression *left, Expression *right)
    : comp_(comp), left_(left), right_(right)
{}

ComparisonExpr::~ComparisonExpr()
{}

RC ComparisonExpr::compare_value(const Value &left, const Value &right, Value &result) const
{
  RC rc = RC::SUCCESS;
  Value cmp_result;
  RC cmp_rc = left.compare(right, cmp_result);
  result.set_null();
  switch (comp_) {
    case EQUAL_TO: {
      if (cmp_rc != RC::SUCCESS) return cmp_rc;
      if (cmp_result.is_null()) break;
      result.set_boolean(cmp_result.get_int() == 0);
    } break;
    case LESS_EQUAL: {
      if (cmp_rc != RC::SUCCESS) return cmp_rc;
      if (cmp_result.is_null()) break;
      result.set_boolean(cmp_result.get_int() <= 0);
    } break;
    case NOT_EQUAL: {
      if (cmp_rc != RC::SUCCESS) return cmp_rc;
      if (cmp_result.is_null()) break;
      result.set_boolean(cmp_result.get_int() != 0);
    } break;
    case LESS_THAN: {
      if (cmp_rc != RC::SUCCESS) return cmp_rc;
      if (cmp_result.is_null()) break;
      result.set_boolean(cmp_result.get_int() < 0);
    } break;
    case GREAT_EQUAL: {
      if (cmp_rc != RC::SUCCESS) return cmp_rc;
      if (cmp_result.is_null()) break;
      result.set_boolean(cmp_result.get_int() >= 0);
    } break;
    case GREAT_THAN: {
      if (cmp_rc != RC::SUCCESS) return cmp_rc;
      if (cmp_result.is_null()) break;
      result.set_boolean(cmp_result.get_int() > 0);
    } break;
    case LIKE_OPER:{
      if (left.is_null() || right.is_null()) break;
      const char *left_value = left.data();   //Uriel  
      const char *like_expr = right.data();   //Ur_e
      result.set_boolean(LIKE_Match(left_value, like_expr));
    } break;
    case NOT_LIKE_OPER: {
      if (left.is_null() || right.is_null()) break;
      const char *left_value = left.data();
      const char *like_expr = right.data();
      result.set_boolean(!(LIKE_Match(left_value,like_expr)));
    } break;
    case IS_OPER: {
      if (left.is_null() || right.is_null()) {
        result.set_boolean(left.is_null() && right.is_null());
      } else {
        // EQUAL_TO
        if (cmp_rc != RC::SUCCESS) return cmp_rc;
        if (cmp_result.is_null()) break;
        result.set_boolean(cmp_result.get_int() == 0);
      }
    } break;
    case IS_NOT_OPER: {
      if (left.is_null() || right.is_null()) {
        result.set_boolean(!(left.is_null() && right.is_null()));
      } else {
        // NOT_EQUAL
        if (cmp_rc != RC::SUCCESS) return cmp_rc;
        if (cmp_result.is_null()) break;
        result.set_boolean(cmp_result.get_int() != 0);
      }
    } break;
    case IN_OPER: { // 如果右操作数不是 value list，就把它单独装进一个 value list；
                    // 左操作数反之（在 Value::compare 的逻辑里）。
                    // NOT IN 和 EXISTS 同理
      if (left.is_null() || right.is_null()) break;
      Value right_list = right;
      if (right.attr_type() != VAL_LIST) {
        right_list = Value::val_list_from(right); // 或者返回 type dismatch? (parser 可能做不到把带括号的单值解析成 VAL_LIST)
      }
      std::vector<Value> val_list = right_list.get_val_list();
      result.set_boolean(false);
      for (Value &val : val_list) {
        Value val_cmp;
        rc = left.compare(val, val_cmp);
        if (rc != RC::SUCCESS) {
          return rc;
        }
        if (val_cmp.is_null()) {
          result.set_null();
          break;
        }
        if (val_cmp.get_int() == 0) {
          result.set_boolean(true);
          break;
        }
      }
    } break;
    case NOT_IN_OPER: {
      if (left.is_null() || right.is_null()) break;
      Value right_list = right;
      if (right.attr_type() != VAL_LIST) {
        right_list = Value::val_list_from(right);
      }
      std::vector<Value> val_list = right_list.get_val_list();
      result.set_boolean(true);
      for (Value &val : val_list) {
        Value val_cmp;
        rc = left.compare(val, val_cmp);
        if (rc != RC::SUCCESS) {
          return rc;
        }
        if (val_cmp.is_null()) {
          result.set_null();
          break;
        }
        if (val_cmp.get_int() == 0) {
          result.set_boolean(false);
          break;
        }
      }
    } break;
    case EXISTS_OPER: { // 单元运算符，只有 right；只看 val_list 是否存在元素，不看元素是否非空
      if (right.is_null()) break;
      Value right_list = right;
      if (right.attr_type() != VAL_LIST) {
        right_list = Value::val_list_from(right);
      }
      std::vector<Value> val_list = right_list.get_val_list();
      result.set_boolean(!val_list.empty());
    } break;
    default: {
      LOG_WARN("unsupported comparison. %d", comp_);
      rc = RC::INTERNAL;
    } break;
  }

  return rc;
}

RC ComparisonExpr::try_get_value(Value &cell) const
{
  RC rc = RC::SUCCESS;

  Value lval;
  rc = left_->try_get_value(lval);
  if (rc != RC::SUCCESS) {
    return rc;
  }
  Value rval;
  rc = right_->try_get_value(rval);
  if (rc != RC::SUCCESS) {
    return rc;
  }
  rc = compare_value(lval, rval, cell);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to compare tuple cells. rc=%s", strrc(rc));
  }

  return rc;
}

RC ComparisonExpr::get_value(const Tuple &tuple, Value &value) const
{
  Value left_value;
  Value right_value;

  RC rc = left_->get_value(tuple, left_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
    return rc;
  }
  rc = right_->get_value(tuple, right_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of right expression. rc=%s", strrc(rc));
    return rc;
  }

  rc = compare_value(left_value, right_value, value);
  return rc;
}

RC ComparisonExpr::init_expr_tree(Session *session, Db *db, const GroupsSqlNode *groups, const std::unordered_map<std::string, Table *> &merged_table_map, const std::vector<ExprInfo> &merged_expr_infos, bool failure_when_found_expr)
{
  RC rc = left_->init_expr_tree(session, db, groups, merged_table_map, merged_expr_infos, failure_when_found_expr);
  if (rc != RC::SUCCESS) return rc;
  rc = right_->init_expr_tree(session, db, groups, merged_table_map, merged_expr_infos, failure_when_found_expr);
  if (rc != RC::SUCCESS) return rc;

  inferred_nullable_ = ExprInfo::default_inferred_nullable;
  inferred_length_ = ExprInfo::default_inferred_length;

  has_aggre_ = left_->has_aggre() || right_->has_aggre();

  return rc;
}

////////////////////////////////////////////////////////////////////////////////
ConjunctionExpr::ConjunctionExpr(Type type, vector<unique_ptr<Expression>> &children)
    : conjunction_type_(type), children_(std::move(children))
{}

ConjunctionExpr::ConjunctionExpr(Type type, Expression *left, Expression *right)
    : conjunction_type_(type)
{
  if (left) {
    children_.push_back(std::unique_ptr<Expression>(left));
  }
  if (right) {
    children_.push_back(std::unique_ptr<Expression>(right));
  }
}

RC ConjunctionExpr::get_value(const Tuple &tuple, Value &value) const
{
  RC rc = RC::SUCCESS;
  if (children_.empty()) {
    value.set_boolean(true);
    return rc;
  }

  bool result = false;
  if (conjunction_type_ == Type::AND) result = true;
  if (conjunction_type_ == Type::OR) result = false;
  Value tmp_value;
  for (const unique_ptr<Expression> &expr : children_) {
    rc = expr->get_value(tuple, tmp_value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to get value by child expression. rc=%s", strrc(rc));
      return rc;
    }
    if (tmp_value.is_null()) {
      value.set_null();
      return rc;
    }
    bool bool_value;
    if (tmp_value.attr_type() == INTS) bool_value = tmp_value.get_int() != 0;
    else bool_value = tmp_value.get_boolean();
    result |= bool_value;
    if ((conjunction_type_ == Type::AND && !bool_value) || (conjunction_type_ == Type::OR && bool_value)) {
      value.set_boolean(bool_value);
      return rc;
    }
  }
  value.set_boolean(result);
  return rc;
}

RC ConjunctionExpr::init_expr_tree(Session *session, Db *db, const GroupsSqlNode *groups, const std::unordered_map<std::string, Table *> &merged_table_map, const std::vector<ExprInfo> &merged_expr_infos, bool failure_when_found_expr)
{
  for (int i = 0; i < children_.size(); ++i)
  {
    RC rc = children_[i]->init_expr_tree(session, db, groups, merged_table_map, merged_expr_infos, failure_when_found_expr);
    if (rc != RC::SUCCESS) return rc;

    has_aggre_ |= children_[i]->has_aggre();
  }

  inferred_nullable_ = ExprInfo::default_inferred_nullable;
  inferred_length_ = ExprInfo::default_inferred_length;

  return RC::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////

ArithmeticExpr::ArithmeticExpr(ArithmeticExpr::Type type, Expression *left, Expression *right)
    : arithmetic_type_(type), left_(left), right_(right)
{}
ArithmeticExpr::ArithmeticExpr(ArithmeticExpr::Type type, unique_ptr<Expression> left, unique_ptr<Expression> right)
    : arithmetic_type_(type), left_(std::move(left)), right_(std::move(right))
{}

AttrType ArithmeticExpr::value_type() const
{
  if (!right_) {
    return left_->value_type();
  }

  if (left_->value_type() == AttrType::NULLS || right_->value_type() == AttrType::NULLS) {
    return AttrType::NULLS;
  }

  if (left_->value_type() == AttrType::INTS &&
      right_->value_type() == AttrType::INTS &&
      arithmetic_type_ != Type::DIV) {
    return AttrType::INTS;
  }
  
  return AttrType::FLOATS;
}

RC ArithmeticExpr::calc_value(const Value &left_value, const Value &right_value, Value &value) const
{
  RC rc = RC::SUCCESS;

  if (left_value.is_null() || right_value.is_null()) {
    value.set_null();
    return rc;
  }

  const AttrType target_type = value_type();

  switch (arithmetic_type_) {
    case Type::ADD: {
      if (target_type == AttrType::INTS) {
        value.set_int(left_value.get_int() + right_value.get_int());
      } else {
        value.set_float(left_value.get_float() + right_value.get_float());
      }
    } break;

    case Type::SUB: {
      if (target_type == AttrType::INTS) {
        value.set_int(left_value.get_int() - right_value.get_int());
      } else {
        value.set_float(left_value.get_float() - right_value.get_float());
      }
    } break;

    case Type::MUL: {
      if (target_type == AttrType::INTS) {
        value.set_int(left_value.get_int() * right_value.get_int());
      } else {
        value.set_float(left_value.get_float() * right_value.get_float());
      }
    } break;

    case Type::DIV: {
      if (target_type == AttrType::INTS) {
        if (right_value.get_int() == 0) {
          // NOTE: 设置为整数最大值是不正确的。通常的做法是设置为NULL，但是当前的miniob没有NULL概念，所以这里设置为整数最大值。
          //value.set_int(numeric_limits<int>::max());
          // DONE null; remove the temporary RC::NULL_VALUE
          //return RC::NULL_VALUE;
          value.set_null();
        } else {
          value.set_int(left_value.get_int() / right_value.get_int());
        }
      } else {
        if (right_value.get_float() > -EPSILON && right_value.get_float() < EPSILON) {
          // NOTE: 设置为浮点数最大值是不正确的。通常的做法是设置为NULL，但是当前的miniob没有NULL概念，所以这里设置为浮点数最大值。
          //value.set_float(numeric_limits<float>::max());
          // DONE null; remove the temporary RC::NULL_VALUE
          //return RC::NULL_VALUE;
          value.set_null();
        } else {
          value.set_float(left_value.get_float() / right_value.get_float());
        }
      }
    } break;

    case Type::NEGATIVE: {
      if (target_type == AttrType::INTS) {
        value.set_int(-left_value.get_int());
      } else {
        value.set_float(-left_value.get_float());
      }
    } break;

    default: {
      rc = RC::INTERNAL;
      LOG_WARN("unsupported arithmetic type. %d", arithmetic_type_);
    } break;
  }
  return rc;
}

RC ArithmeticExpr::get_value(const Tuple &tuple, Value &value) const
{
  RC rc = RC::SUCCESS;

  Value left_value;
  Value right_value;

  rc = left_->get_value(tuple, left_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
    return rc;
  }
  if (right_) {
    rc = right_->get_value(tuple, right_value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to get value of right expression. rc=%s", strrc(rc));
      return rc;
    }
  }
  return calc_value(left_value, right_value, value);
}

RC ArithmeticExpr::init_expr_tree(Session *session, Db *db, const GroupsSqlNode *groups, const std::unordered_map<std::string, Table *> &merged_table_map, const std::vector<ExprInfo> &merged_expr_infos, bool failure_when_found_expr)
{
  RC rc = left_->init_expr_tree(session, db, groups, merged_table_map, merged_expr_infos, failure_when_found_expr);
  if (rc != RC::SUCCESS) return rc;
  has_aggre_ = left_->has_aggre();
  if (right_) {
    rc = right_->init_expr_tree(session, db, groups, merged_table_map, merged_expr_infos, failure_when_found_expr);
    if (rc != RC::SUCCESS) return rc;
    has_aggre_ |= right_->has_aggre();
  }

  inferred_nullable_ = ExprInfo::default_inferred_nullable;
  inferred_length_ = ExprInfo::default_inferred_length;

  return RC::SUCCESS;
}

RC ArithmeticExpr::try_get_value(Value &value) const
{
  RC rc = RC::SUCCESS;

  Value left_value;
  Value right_value;

  rc = left_->try_get_value(left_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
    return rc;
  }

  if (right_) {
    rc = right_->try_get_value(right_value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to get value of right expression. rc=%s", strrc(rc));
      return rc;
    }
  }

  return calc_value(left_value, right_value, value);
}

FunctionExpr::FunctionExpr(Type type, std::vector<Expression *> args)
  : function_type_(type) {
  // init args_ by copying args into a vector of unique ptrs
  args_.reserve(args.size());
  for (Expression* expr : args) {
    std::unique_ptr<Expression> uni_expr(expr);
    args_.push_back(std::move(uni_expr));
  }
}

FunctionExpr::FunctionExpr(Type type, std::vector<std::unique_ptr<Expression>> &args)
  : function_type_(type), args_(std::move(args))
{}

AttrType FunctionExpr::value_type() const
{
  for (const auto &arg : args_) {
    if (arg->value_type() == AttrType::NULLS) {
      return AttrType::NULLS;
    }
  }

  switch (function_type_) {
    case Type::LENGTH:
      return AttrType::INTS;
    case Type::ROUND:
      return AttrType::FLOATS;
    case Type::DATE_FORMAT:
      return AttrType::CHARS;
    case Type::NOT:
      return AttrType::BOOLEANS;
    default:
      LOG_WARN("unsupported function type (%d)", function_type_);
      return AttrType::UNDEFINED;
  }
}

RC FunctionExpr::get_value(const Tuple &tuple, Value &value) const
{
  RC rc = RC::SUCCESS;

  Value arg0_value, arg1_value;
  if (args_.size() == 0) return RC::INVALID_ARGUMENT;
  rc = args_[0]->get_value(tuple, arg0_value);
  if (args_.size() > 1) {
    rc = args_[1]->get_value(tuple, arg1_value);
  }
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of args. rc=%s", strrc(rc));
    return rc;
  }

  return eval_value(arg0_value, arg1_value, value);
}

RC FunctionExpr::init_expr_tree(Session *session, Db *db, const GroupsSqlNode *groups, const std::unordered_map<std::string, Table *> &merged_table_map, const std::vector<ExprInfo> &merged_expr_infos, bool failure_when_found_expr)
{
  for (int i = 0; i < args_.size(); ++i)
  {
    RC rc = args_[i]->init_expr_tree(session, db, groups, merged_table_map, merged_expr_infos, failure_when_found_expr);
    if (rc != RC::SUCCESS) return rc;

    has_aggre_ |= args_[i]->has_aggre();
  }

  inferred_nullable_ = ExprInfo::default_inferred_nullable;
  inferred_length_ = ExprInfo::default_inferred_length;

  return RC::SUCCESS;
}

RC FunctionExpr::try_get_value(Value &value) const
{
  RC rc = RC::SUCCESS;

  Value arg0_value, arg1_value;
  if (args_.size() == 0) return RC::INVALID_ARGUMENT;
  rc = args_[0]->try_get_value(arg0_value);
  if (args_.size() > 1) {
    rc = args_[1]->try_get_value(arg1_value);
  }
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of args. rc=%s", strrc(rc));
    return rc;
  }

  return eval_value(arg0_value, arg1_value, value);
}

RC FunctionExpr::eval_value(const Value &arg0_value, const Value &arg1_value, Value &value) const
{
  RC rc = RC::SUCCESS;

  switch (function_type_) {
    case Type::LENGTH:
      return eval_length(arg0_value, value);
    case Type::ROUND:
      return eval_round(arg0_value, arg1_value, value);
    case Type::DATE_FORMAT:
      return eval_date_format(arg0_value, arg1_value, value);
    case Type::NOT:
      return eval_not(arg0_value, value);
    default:
      LOG_WARN("unsupported function type (%d)", function_type_);
      return RC::INTERNAL;
  }
}

RC FunctionExpr::eval_not(const Value &arg_value, Value &value) const
{
  RC rc = RC::SUCCESS;

  if (arg_value.is_null()) {
    value.set_null();
    return rc;
  }
  if (arg_value.attr_type() != BOOLEANS) {
    Value conv_arg_value;
    rc = arg_value.convert_into(BOOLEANS, conv_arg_value);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    if (conv_arg_value.is_null()) {
      value.set_null();
      return rc;
    }
    value.set_boolean(!conv_arg_value.get_boolean());
  } else {
    value.set_boolean(!arg_value.get_boolean());
  }
  return rc;
}

RC FunctionExpr::eval_length(const Value &arg_value, Value &value) const
{
  if (arg_value.attr_type() != AttrType::CHARS) {
    LOG_WARN("LENGTH function expects a string argument.");
    return RC::INVALID_ARGUMENT;
  }

  int length = arg_value.get_string().length();
  value.set_int(length);
  return RC::SUCCESS;
}

RC FunctionExpr::eval_round(const Value &arg0_value, const Value &arg1_value, Value &value) const
{
  Value digits_value = arg1_value;
  if (digits_value.attr_type() == AttrType::UNDEFINED) {
    digits_value.set_int(0);
  }
  if ((arg0_value.attr_type() != AttrType::FLOATS && arg0_value.attr_type() != AttrType::INTS) || digits_value.attr_type() != AttrType::INTS) {
    LOG_WARN("ROUND function expects a float or integer argument and an optional integer argument.");
    return RC::INVALID_ARGUMENT;
  }

  float multiplier = std::pow(10, digits_value.get_int());
  float rounded = std::round(arg0_value.get_float() * multiplier) / multiplier;
  value.set_float(rounded);
  return RC::SUCCESS;
}

RC FunctionExpr::eval_date_format(const Value &arg0_value, const Value &arg1_value, Value &value) const
{
  if (arg0_value.attr_type() != AttrType::DATES || arg1_value.attr_type() != AttrType::CHARS){
    LOG_WARN("FORMAT function expects a date argument and a string argument.");
    return RC::INVALID_ARGUMENT;
  }

  struct tm tm = { };
  auto date = arg0_value.get_date();
  tm.tm_year = date / 10000 - 1900; // tm_year since 1900
  tm.tm_mon = (date / 100) % 100 - 1; // tm_mon range 0-11
  tm.tm_mday = date % 100;
  //std::time_t t = static_cast<std::time_t>(arg0_value.get_date());
  std::time_t t = mktime(&tm);
  /*char buf[1024];
  std::strftime(buf, sizeof(buf), arg1_value.get_string().c_str(), std::localtime(&t));
  value.set_string(buf, strlen(buf));*/
  /*std::ostringstream oss;
  oss << std::put_time(std::localtime(&t), arg1_value.get_string().c_str());
  std::string str = oss.str();*/
  int year = 1900 + tm.tm_year;
  int month = tm.tm_mon + 1;
  int day = tm.tm_mday;
  std::string str = arg1_value.get_string();
  std::string month_str;
  switch (tm.tm_mon) {
    case 0: month_str = "January"; break;
    case 1: month_str = "February"; break;
    case 2: month_str = "March"; break;
    case 3: month_str = "April"; break;
    case 4: month_str = "May"; break;
    case 5: month_str = "June"; break;
    case 6: month_str = "July"; break;
    case 7: month_str = "August"; break;
    case 8: month_str = "September"; break;
    case 9: month_str = "October"; break;
    case 10: month_str = "November"; break;
    case 11: month_str = "December"; break;
  }
  std::string day_str = std::to_string(day);
  if (day / 10 == 1) {
    day_str += "th";
  } else {
    switch (day % 10) {
      case 1: day_str += "st"; break;
      case 2: day_str += "nd"; break;
      case 3: day_str += "rd"; break;
      default: day_str += "th"; break;
    }
  }
  std::string y_str = (year % 100 < 10 ? "0" : "") + std::to_string(year % 100);
  std::string m_str = (month % 100 < 10 ? "0" : "") + std::to_string(month % 100);
  std::string d_str = (day % 100 < 10 ? "0" : "") + std::to_string(day % 100);
  str = std::regex_replace(str, std::regex("%Y"), std::to_string(year).c_str());
  str = std::regex_replace(str, std::regex("%M"), month_str.c_str());
  str = std::regex_replace(str, std::regex("%D"), day_str.c_str());
  str = std::regex_replace(str, std::regex("%y"), y_str.c_str());
  str = std::regex_replace(str, std::regex("%m"), m_str.c_str());
  str = std::regex_replace(str, std::regex("%d"), d_str.c_str());
  str = std::regex_replace(str, std::regex("%"), "");
  value.set_string(str.c_str(), str.length());
  return RC::SUCCESS;
}

RC SubqueryExpr::get_value(const Tuple &tuple, Value &value) const
{
  std::unique_ptr<PhysicalOperator> oper;
  RC rc = generate_oper(&tuple, oper);
  if (rc != RC::SUCCESS) return rc;

  std::vector<Value> query_result;
  rc = execute_query(oper, query_result);
  if (rc != RC::SUCCESS) return rc;

  value.set_val_list(query_result);
  return rc;
}

RC SubqueryExpr::init_expr_tree(Session *session, Db *db, const GroupsSqlNode *groups, const std::unordered_map<std::string, Table *> &merged_table_map, const std::vector<ExprInfo> &merged_expr_infos, bool failure_when_found_expr)
{
  session_ = session;
  db_ = db;
  // Deep copy merged_table_map and merged_expressions
  for (const auto &pair : merged_table_map) {
    merged_table_map_.insert(pair);
  }
  for (const auto expr_info : merged_expr_infos) {
    merged_expr_infos_.push_back(expr_info);
  }

  inferred_nullable_ = ExprInfo::default_inferred_nullable;
  inferred_length_ = ExprInfo::default_inferred_length;

  has_aggre_ = false;

  return RC::SUCCESS;
}

RC SubqueryExpr::try_get_value(Value &value) const
{
  std::unique_ptr<PhysicalOperator> oper;
  RC rc = generate_oper(nullptr, oper);
  if (rc != RC::SUCCESS) return rc;

  std::vector<Value> query_result;
  rc = execute_query(oper, query_result);
  if (rc != RC::SUCCESS) return rc;

  value.set_val_list(query_result);
  return rc;
}

RC SubqueryExpr::generate_oper(const Tuple *outer_query_tuple, std::unique_ptr<PhysicalOperator> &oper) const
{
  RC rc = RC::SUCCESS;

  // 因为 Expression 的 unique_ptr 很烦人，要执行多次子查询，可能只有每次都从 parse 开始进行全套流程了，除非实现了深度复制 Expression
  // 我们姑且认为这个预处理过程是 O(1) 并且常数不大，所以能实现就行，因为子查询至少 O(n)

  ParsedSqlResult parsed_sql_result;
  parse((sql_ + ";").c_str(), &parsed_sql_result);
  if (parsed_sql_result.sql_nodes().size() != 1) {
    return RC::INTERNAL;
  }
  std::unique_ptr<ParsedSqlNode> sql_node = std::move(parsed_sql_result.sql_nodes().front());
  if (sql_node->flag == SCF_ERROR) {
    rc = RC::SQL_SYNTAX;
    session_->current_request()->sql_result()->set_return_code(rc);
    session_->current_request()->sql_result()->set_state_string("Failed to parse sql");
    return RC::SQL_SYNTAX;
  }
  SelectSqlNode select_sql_node = sql_node->selection;

  Stmt *stmt = nullptr;
  // MySQL 不支持子查询访问外部查询定义的别名，所以得去掉这个传递了。。。
  // 换种思路，这里继续支持，但是 init_expr_tree 中探测到非法情况时报错即可
  rc = SelectStmt::create(session_, db_, select_sql_node, stmt, &merged_table_map_, &merged_expr_infos_);
  //rc = SelectStmt::create(session_, db_, select_sql_node, stmt, &merged_table_map_, nullptr);
  if (rc != RC::SUCCESS) return rc;
  static_cast<SelectStmt *>(stmt)->outer_query_tuple = outer_query_tuple;

  expr_infos_ = static_cast<SelectStmt *>(stmt)->expr_infos();

  LogicalPlanGenerator  logical_plan_generator_;
  PhysicalPlanGenerator physical_plan_generator_;
  Rewriter              rewriter_;

  unique_ptr<LogicalOperator> logical_operator;
  rc = logical_plan_generator_.create(stmt, logical_operator);
  if (rc != RC::SUCCESS) return rc;

#if false // REWRITE_DISABLE // TODO re-enable
  bool change_made = false;
  do {
    change_made = false;
    rc = rewriter_.rewrite(logical_operator, change_made);
    if (rc != RC::SUCCESS) return rc;
  } while (change_made);
#endif

  unique_ptr<PhysicalOperator> physical_operator;
  rc = physical_plan_generator_.create(*logical_operator, physical_operator);
  if (rc != RC::SUCCESS) return rc;

  oper = std::move(physical_operator);

  return rc;
}

RC SubqueryExpr::execute_query(std::unique_ptr<PhysicalOperator> &oper, std::vector<Value> &query_result) const
{
  RC rc = RC::SUCCESS;
  rc = oper->open(session_->current_trx());
  if (rc != RC::SUCCESS) {
    oper->close();
    return rc;
  }
  Tuple *tuple = nullptr;
  query_result.clear();
  while (true) {
    // next tuple
    rc = oper->next();
    if (rc != RC::SUCCESS) break;
    tuple = oper->current_tuple();
    if (rc != RC::SUCCESS) break;
    assert(tuple != nullptr);

    // store current tuple
    std::vector<Value> tuple_values;
    int cell_num = tuple->cell_num();
    tuple_values.reserve(cell_num);
    for (int i = 0; i < cell_num; i++) {
      Value value;
      rc = tuple->cell_at(i, value);
      if (rc != RC::SUCCESS) {
        oper->close();
        break;
      }
      tuple_values.push_back(value);
    }
    if (rc != RC::SUCCESS) break;
    query_result.push_back(Value(tuple_values));
  }

  if (rc == RC::RECORD_EOF) {
    rc = RC::SUCCESS;
  } else if (OB_FAIL(rc)) {
    LOG_ERROR("Subquery terminated unsuccessfully. ret=%s, error=%s", strrc(rc), strerror(errno));
    oper->close();
  }

  return rc;
}

bool LIKE_Match(const char *str,const char *pattern){

  std::regex reg("_");
  std::string s2 = std::regex_replace(std::string(pattern), reg, ".");

  reg = "%";
  s2 = std::regex_replace(s2, reg, ".*");

  std::regex patternObj(s2);
  return std::regex_match(std::string(str), patternObj);
}

RC AggreExpr::get_value(const Tuple &tuple, Value &value) const
{
  // 对除了 NONE 之外的聚合函数来说，get_value(tuple, value) 用来累积结果，所以 value 象征性地传一个空值
  value.set_null();

  Value arg_value;
  RC rc = args_[0]->get_value(tuple, arg_value);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  if (aggre_type_ == Type::NONE) {
    value.set_value(arg_value); // NONE 类型的 get_value 当成正常的表达式来用
  }

  std::vector<Value> group_values;
  if (!groups_.group_exprs.empty()) {
    for (const auto &expr : groups_.group_exprs) {
      Value group_value;
      rc = expr->get_value(tuple, group_value);
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to get group values");
        return rc;
      }
      group_values.push_back(group_value);
    }
  }
  if (aggre_results_.count(group_values) == 0) {
    aggre_results_.insert(std::pair<std::vector<Value>, AggreResult>(group_values, AggreResult()));
  }
  AggreResult &group_result = aggre_results_.at(group_values);

  // 遇到 NULL，不累积 cnt，也不计算 sum、max_val、min_val，跳过
  if (arg_value.is_null()) {
    return rc;
  }

  if (aggre_type_ == Type::SUM || aggre_type_ == Type::AVG) {
    float float_val = 0;
    if (arg_value.attr_type() == INTS) {
      float_val = arg_value.get_int();
    } else if (arg_value.attr_type() == FLOATS) {
      float_val = arg_value.get_float();
    } else {
      return RC::SCHEMA_FIELD_TYPE_MISMATCH; // 如果有类型问题，应该在 init_expr_tree 中就返回这个 RC 了
    }
    group_result.sum += float_val;
  }
  ++group_result.cnt;
  if (group_result.first_val.is_null()) {
    group_result.first_val.set_value(arg_value);
  }
  if (group_result.max_val.is_null()) {
    group_result.max_val.set_value(arg_value);
  } else {
    Value cmp_result;
    rc = group_result.max_val.compare(arg_value, cmp_result);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    if (cmp_result.is_null()) {
      return RC::INTERNAL;
    }
    if (cmp_result.get_int() < 0) {
      group_result.max_val.set_value(arg_value);
    }
  }
  if (group_result.min_val.is_null()) {
    group_result.min_val.set_value(arg_value);
  } else {
    Value cmp_result;
    rc = group_result.min_val.compare(arg_value, cmp_result);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    if (cmp_result.is_null()) {
      return RC::INTERNAL;
    }
    if (cmp_result.get_int() > 0) {
      group_result.min_val.set_value(arg_value);
    }
  }

  return rc;
}

RC AggreExpr::init_expr_tree(Session *session, Db *db, const GroupsSqlNode *groups, const std::unordered_map<std::string, Table *> &merged_table_map, const std::vector<ExprInfo> &merged_expr_infos, bool failure_when_found_expr)
{
  has_aggre_ = true;

  if (args_.size() != 1) return RC::INVALID_ARGUMENT;

  if (groups) {
    groups_ = *groups;
  }

  const std::unique_ptr<Expression> &arg = args_[0];

   // 对于 COUNT(*)，特殊处理，把 * 等效替换为一个的非 NULL 常量
   if (arg->type() == ExprType::FIELD) {
    FieldExpr *field_arg_ = static_cast<FieldExpr *>(arg.get());
    if (field_arg_->name() == "*") {
      if (aggre_type_ != Type::CNT) {
        return RC::SCHEMA_FIELD_MISSING;
      }
      args_[0] = std::make_unique<ValueExpr>(Value(0));
      return RC::SUCCESS;
    }
   }

  // 检查是否有嵌套的聚合函数
  RC rc = arg->init_expr_tree(session, db, groups, merged_table_map, merged_expr_infos, failure_when_found_expr);
  if (rc != RC::SUCCESS) return rc;
  if (arg->has_aggre()) {
    LOG_WARN("Nested aggregation functions are illegal");
    return RC::SCHEMA_FIELD_MISSING;
  }

  // 类型合法性判断
  switch (aggre_type_) {
    case Type::CNT:
    case Type::MAX:
    case Type::MIN:
    case Type::NONE:
    {
    } break;
    case Type::SUM:
    case Type::AVG:
    {
      if (arg->value_type() != INTS && arg->value_type() != FLOATS) {
        return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }
    } break;
    default: {
      return RC::INTERNAL;
    }
  }

  inferred_nullable_ = ExprInfo::default_inferred_nullable;
  inferred_length_ = ExprInfo::default_inferred_length;

  has_aggre_ = true;

  return RC::SUCCESS;
}

RC AggreExpr::try_get_value(Value &value) const {
  // try_get_value 用来取当前组的聚合函数计算结果，并更新 group，为下一组的计算作准备

  auto iter = aggre_results_.begin();
  if (iter == aggre_results_.end()) {
    return RC::RECORD_EOF;
  }
  for (int i = 0; i < group_index_; ++i) {
    ++iter;
    if (iter == aggre_results_.end()) {
      return RC::RECORD_EOF;
    }
  }

  const AggreResult &group_result = iter->second;

  switch (aggre_type_) {
    case Type::CNT: {
      value.set_int(group_result.cnt);
    } break;
    case Type::SUM: {
      if (group_result.cnt == 0) {
        value.set_null();
      } else {
        value.set_float(group_result.sum);
      }
    } break;
    case Type::AVG: {
      if (group_result.cnt == 0) {
        value.set_null();
      } else {
        value.set_float(group_result.sum / group_result.cnt);
      }
    } break;
    case Type::MAX: {
      value.set_value(group_result.max_val);
    } break;
    case Type::MIN: {
      value.set_value(group_result.min_val);
    } break;
    case Type::NONE: {
      value.set_value(group_result.first_val);
    } break;
    default: {
      return RC::INTERNAL;
    }
  }

  ++group_index_;

  return RC::SUCCESS;
}

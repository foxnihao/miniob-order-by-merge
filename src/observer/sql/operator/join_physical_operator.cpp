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
// Created by WangYunlai on 2022/12/30.
//

#include "sql/operator/join_physical_operator.h"

NestedLoopJoinPhysicalOperator::NestedLoopJoinPhysicalOperator()
{}

RC NestedLoopJoinPhysicalOperator::open(Trx *trx)
{
  if (children_.size() != 2) {
    LOG_WARN("nlj operator should have 2 children");
    return RC::INTERNAL;
  }

  RC rc = RC::SUCCESS;
  left_ = children_[0].get();
  right_ = children_[1].get();
  right_closed_ = true;
  round_done_ = true;

  rc = left_->open(trx);
  trx_ = trx;
  return rc;
}

RC NestedLoopJoinPhysicalOperator::next()
{
  RC rc = RC::SUCCESS;

  bool left_need_step = (left_tuple_ == nullptr) || round_done_;

  while (true) {
    while (!left_need_step) {
      rc = right_next();
      if (rc != RC::SUCCESS) {
        if (rc == RC::RECORD_EOF) {
          left_need_step = true;
        } else {
          return rc;
        }
      } else {
        return rc;
      }
    }

    rc = left_next();
    if (rc != RC::SUCCESS) {
      return rc;
    }
    left_need_step = false;
  }

  return rc;
}

RC NestedLoopJoinPhysicalOperator::close()
{
  RC rc = left_->close();
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to close left oper. rc=%s", strrc(rc));
  }

  if (!right_closed_) {
    rc = right_->close();
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to close right oper. rc=%s", strrc(rc));
    } else {
      right_closed_ = true;
    }
  }
  return rc;
}

Tuple *NestedLoopJoinPhysicalOperator::current_tuple()
{
  JoinedTuple *tuple = new JoinedTuple(joined_tuple_);
  return tuple;
}

RC NestedLoopJoinPhysicalOperator::left_next()
{
  RC rc = RC::SUCCESS;
  rc = left_->next();
  if (rc != RC::SUCCESS) {
    return rc;
  }

  left_tuple_ = left_->current_tuple();
  joined_tuple_.set_left(left_tuple_);
  return rc;
}

RC NestedLoopJoinPhysicalOperator::right_next()
{
  RC rc = RC::SUCCESS;
  if (round_done_) {
    if (!right_closed_) {
      rc = right_->close();
      right_closed_ = true;
      if (rc != RC::SUCCESS) {
        return rc;
      }
    }

    rc = right_->open(trx_);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    right_closed_ = false;

    round_done_ = false;
  }

  bool condition_bool = false;
  do {
    rc = right_->next();
    if (rc != RC::SUCCESS) {
      if (rc == RC::RECORD_EOF) {
        round_done_ = true;
      }
      return rc;
    }
    right_tuple_ = right_->current_tuple();
    joined_tuple_.set_right(right_tuple_);

    if (condition == nullptr) {
      condition_bool = true;
    } else {
      Value condition_value;
      rc = condition->get_value(joined_tuple_, condition_value);
      if (rc != RC::SUCCESS) {
        LOG_WARN("join operator cannot get join condition value from tuple");
        return rc;
      }

      if (condition_value.attr_type() != BOOLEANS) {
        Value converted_value;
        rc = condition_value.convert_into(BOOLEANS, converted_value);
        if (rc != RC::SUCCESS) {
          LOG_WARN("join condition got a value inconvertible to BOOLEAN");
          return rc;
        }
        condition_value.set_value(converted_value);
      }

      condition_bool = condition_value.is_null() ? false : condition_value.get_boolean();
    }
  } while (!condition_bool);

  return rc;
}

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

#include "common/rc.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "sql/stmt/filter_stmt.h"
#include "storage/db/db.h"
#include "storage/table/table.h"

FilterStmt::~FilterStmt() {}

RC FilterStmt::create(Session *session, Db *db, std::unique_ptr<Expression> expression, FilterStmt *&stmt)
{
  RC rc = RC::SUCCESS;

  if (expression) {
    if (!Value::can_convert(expression->value_type(), BOOLEANS)) {
      LOG_WARN("filter expression should be convertible to BOOLEAN");
      return RC::SCHEMA_FIELD_TYPE_MISMATCH;
    }
  }

  FilterStmt *filter_stmt = new FilterStmt();
  filter_stmt->expression_ = std::move(expression);

  stmt = filter_stmt;
  return rc;
}

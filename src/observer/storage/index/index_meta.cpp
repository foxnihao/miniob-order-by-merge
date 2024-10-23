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
// Created by Wangyunlai.wyl on 2021/5/18.
//

#include "storage/index/index_meta.h"
#include "storage/field/field_meta.h"
#include "storage/table/table_meta.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "json/json.h"

const static Json::StaticString FIELD_NAME("name");
const static Json::StaticString FIELD_FIELD_NAME("field_name");
const static Json::StaticString FIELD_IS_UNIQUE("is_unique");

RC IndexMeta::init(const char *name, const std::vector<const FieldMeta *> field, bool is_unique)
{
  if (common::is_blank(name)) {
    LOG_ERROR("Failed to init index, name is empty.");
    return RC::INVALID_ARGUMENT;
  }

  name_ = name;
  for (const FieldMeta *it: field) 
    field_.push_back(it->name());

  is_unique_ = is_unique;
  return RC::SUCCESS;
}

void IndexMeta::to_json(Json::Value &json_value) const
{
  json_value[FIELD_NAME] = name_;

  for(std::string field : field_ )
    json_value[FIELD_FIELD_NAME].append(field);

  json_value[FIELD_IS_UNIQUE] = is_unique_;
}

RC IndexMeta::from_json(const TableMeta &table, const Json::Value &json_value, IndexMeta &index)
{
  const Json::Value &name_value = json_value[FIELD_NAME];
  const Json::Value &field_value = json_value[FIELD_FIELD_NAME];
  const Json::Value &is_unique_value = json_value[FIELD_IS_UNIQUE];

  if (!name_value.isString()) {
    LOG_ERROR("Index name is not a string. json value=%s", name_value.toStyledString().c_str());
    return RC::INTERNAL;
  }

  if (!field_value.isArray()) {
    LOG_ERROR("Field name of index [%s] is not a string. json value=%s",
        name_value.asCString(),
        field_value.toStyledString().c_str());
    return RC::INTERNAL;
  }

  if (!is_unique_value.isBool()) {
    LOG_ERROR("Is unique of index [%s] is not a int. json value=%s",
        name_value.asCString(),
        is_unique_value.toStyledString().c_str());
    return RC::INTERNAL;
  }

  /*const FieldMeta *field = table.field(field_value.asCString());
  if (nullptr == field) {
    LOG_ERROR("Deserialize index [%s]: no such field: %s", name_value.asCString(), field_value.asCString());
    return RC::SCHEMA_FIELD_MISSING;
  }*/

  /*std::vector<const FieldMeta* > fields;
  for (int i = 0; i < field_value.size(); i++) {
    const FieldMeta *field = table.field(field_value[i].asCString());
    if (nullptr == field) {
      LOG_ERROR("Deserialize index [%s]: no such field: %s", name_value.asCString(), field_value.asCString());
      return RC::SCHEMA_FIELD_MISSING;
    }
  }*/
  std::vector<const FieldMeta*> fields;
  for (int i = 0; i < field_value.size(); i++) {
  const Json::Value &field_name_value = field_value[i];
  if (!field_name_value.isString()) {
    LOG_ERROR("Field name of index [%s] is not a string. json value=%s",
        name_value.asCString(),
        field_name_value.toStyledString().c_str());
    return RC::INTERNAL;
  }
  const FieldMeta *field = table.field(field_name_value.asCString());
  if (nullptr == field) {
    LOG_ERROR("Deserialize index [%s]: no such field: %s", name_value.asCString(), field_name_value.asCString());
    return RC::SCHEMA_FIELD_MISSING;
  }
  fields.push_back(field);
}

  return index.init(name_value.asCString(), fields, is_unique_value.asBool());
}

const char *IndexMeta::name() const
{
  return name_.c_str();
}

std::vector<const char *> IndexMeta::field() const
{
  std::vector<const char *> fields;
  for (std::string it : field_)
    fields.push_back(it.c_str());

  return fields;
}

bool IndexMeta::is_unique() const
{
  return is_unique_;
}

void IndexMeta::desc(std::ostream &os) const
{
  os << "index name=" << name_ << ", field="; 
  for(std::string it : field_ )
    os << it << ", ";
  os << "is_unique=" << is_unique_ << std::endl;
}
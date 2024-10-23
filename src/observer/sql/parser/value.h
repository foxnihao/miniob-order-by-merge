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
// Created by Wangyunlai 2023/6/27
//

#pragma once

#include <string>
#include <vector>
#include <cstring>
#include "common/rc.h"

/**
 * @brief 属性的类型
 * 
 */
enum AttrType
{
  UNDEFINED,
  CHARS,          ///< 字符串类型
  INTS,           ///< 整数类型(4字节)
  FLOATS,         ///< 浮点数类型(4字节)
  DATES,          ///< 日期
  BOOLEANS,       ///< boolean类型，当前不是由parser解析出来的，是程序内部使用的
  NULLS,          ///< null类型
  VAL_LIST,       ///< std::vector<Value>
};

const char *attr_type_to_string(AttrType type);
AttrType attr_type_from_string(const char *s);

#define VALUE_CMP_ERR_RESULT -2
#define VALUE_CMP_NULL_RESULT -3
/**
 * @brief 属性的值
 * 
 */
class Value 
{
public:
  Value() = default;

  Value(AttrType attr_type, char *data, int length = 4) : attr_type_(attr_type)
  {
    this->set_data(data, length);
  }

  explicit Value(int val);
  explicit Value(float val);
  explicit Value(bool val);
  explicit Value(const char *date, bool is_date);
  explicit Value(const char *s, int len = 0);
  explicit Value(const std::vector<Value> &val_list);

  Value(const Value &other) = default;
  Value &operator=(const Value &other) = default;

  void set_type(AttrType type)
  {
    this->attr_type_ = type;
  }
  void set_data(char *data, int length);
  void set_data(const char *data, int length)
  {
    this->set_data(const_cast<char *>(data), length);
  }
  void set_int(int val);
  void set_float(float val);
  void set_boolean(bool val);
  void set_string(const char *s, int len = 0);
  void set_date(const char *s, bool is_date);
  void set_date(int date, bool is_date);
  void set_val_list(const std::vector<Value> &val_list);
  void set_value(const Value &value);
  void set_null();

  static Value val_list_from(const Value &value) {
    std::vector<Value> val_list;
    val_list.push_back(value);
    return Value(val_list);
  }

  std::string to_string() const;

  RC compare(const Value &other, Value &result) const; // result 可能为整数（-1, 0, 1）或 NULL
  RC convert_into(AttrType type, Value &dest) const;
  static bool can_convert(AttrType src_type, AttrType dest_type);

  const char *data() const;
  int length() const
  {
    return length_;
  }

  AttrType attr_type() const
  {
    return attr_type_;
  }

  static bool is_null(char *data, int length);
  bool is_null() const { return is_null_; }
  static Value null_value()
  {
    Value value;
    value.set_null();
    return value;
  }
public:
  /**
   * 获取对应的值
   * 如果当前的类型与期望获取的类型不符，就会执行转换操作
   */
  int get_int() const;
  float get_float() const;
  std::string get_string() const;
  int get_date() const;
  bool get_boolean() const;
  const std::vector<Value> &get_val_list() const;

private:
  AttrType attr_type_ = UNDEFINED;
  int length_ = 0;

  union {
    int int_value_;
    float float_value_;
    bool bool_value_;
    int date_value_;
  } num_value_;
  std::string str_value_;
  bool is_null_ = false;
  std::vector<Value> val_list_value_;
  std::vector<Value> val_list_value_for_null_; // 就是一个空 vector
};
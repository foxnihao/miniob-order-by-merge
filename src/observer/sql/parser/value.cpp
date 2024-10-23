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
// Created by WangYunlai on 2023/06/28.
//

#include <sstream>
#include <cmath>
#include "sql/parser/value.h"
#include "storage/field/field.h"
#include "common/log/log.h"
#include "common/lang/comparator.h"
#include "common/lang/string.h"

const char *ATTR_TYPE_NAME[] = {"undefined", "chars", "ints", "floats", "dates", "booleans", "nulls", "val_list"};

const char *attr_type_to_string(AttrType type)
{
  if (type >= UNDEFINED && type <= VAL_LIST) {
    return ATTR_TYPE_NAME[type];
  }
  return "unknown";
}
AttrType attr_type_from_string(const char *s)
{
  for (unsigned int i = 0; i < sizeof(ATTR_TYPE_NAME) / sizeof(ATTR_TYPE_NAME[0]); i++) {
    if (0 == strcmp(ATTR_TYPE_NAME[i], s)) {
      return (AttrType)i;
    }
  }
  return UNDEFINED;
}

Value::Value(int val)
{
  set_int(val);
}

Value::Value(float val)
{
  set_float(val);
}

Value::Value(bool val)
{
  set_boolean(val);
}

Value::Value(const char *s, int len /*= 0*/)
{
  set_string(s, len);
}

Value::Value(const char *date, bool is_date)
{
  set_date(date, is_date);
}

bool Value::is_null(char *data, int length)
{
  // "\n\n\n\n"
  for (int i = 0; i < std::min(length, 4); ++i) {
    if (data[i] != '\n') return false;
  }
  return true;
}

Value::Value(const std::vector<Value> &val_list)
{
  set_val_list(val_list);
}

void Value::set_data(char *data, int length)
{ 
  if (is_null(data, length)) {
    set_null();
    return;
  }
  switch (attr_type_) {
    case VAL_LIST: {
      val_list_value_ = *(std::vector<Value> *)data;
    } break;
    case CHARS: {
      set_string(data, length);
    } break;
    case INTS: {
      num_value_.int_value_ = *(int *)data;
      length_ = length;
    } break;
    case FLOATS: {
      num_value_.float_value_ = *(float *)data;
      length_ = length;
    } break;
    case BOOLEANS: {
      num_value_.bool_value_ = *(int *)data != 0;
      length_ = length;
    } break;
    case DATES: {
      num_value_.int_value_ = *(int *)data;
      length_ = length;
    } break;
    default: {
      LOG_WARN("unknown data type: %d", attr_type_);
    } break;
  }
}

void Value::set_int(int val)
{
  is_null_ = false;
  attr_type_ = INTS;
  num_value_.int_value_ = val;
  length_ = sizeof(val);
}
void Value::set_float(float val)
{
  is_null_ = false;
  attr_type_ = FLOATS;
  num_value_.float_value_ = val;
  length_ = sizeof(val);
}
void Value::set_boolean(bool val)
{
  is_null_ = false;
  attr_type_ = BOOLEANS;
  num_value_.bool_value_ = val;
  length_ = sizeof(val);
}
void Value::set_string(const char *s, int len /*= 0*/)
{
  is_null_ = false;
  attr_type_ = CHARS;
  if (len > 0) {
    len = strnlen(s, len);
    str_value_.assign(s, len);
  } else {
    str_value_.assign(s);
  }
  length_ = str_value_.length();
}
void Value::set_date(const char *date, bool is_date)
{
  is_null_ = false;
  attr_type_ = DATES;
  int y,m,d;
  sscanf(date, "%*c%d-%d-%d%*c", &y, &m, &d);//not check return value eq 3, lex guarantee
  int dv = y*10000+m*100+d;
  num_value_.date_value_ = dv;
  length_ = sizeof(num_value_.date_value_);
}
void Value::set_date(int date, bool is_date)
{
  is_null_ = false;
  attr_type_ = DATES;
  num_value_.date_value_ = date;
  length_ = sizeof(num_value_.date_value_);
}
void Value::set_val_list(const std::vector<Value> &val_list)
{
  is_null_ = false;
  attr_type_ = VAL_LIST;
  val_list_value_ = val_list;
  length_ = sizeof(val_list);
}
void Value::set_null(){
  is_null_ = true;
  attr_type_ = NULLS;
}

void Value::set_value(const Value &value)
{
  if (value.is_null()) {
    set_null();
    return;
  }
  
  switch (value.attr_type_) {
    case VAL_LIST: {
      set_val_list(value.get_val_list());
    } break;
    case INTS: {
      set_int(value.get_int());
    } break;
    case FLOATS: {
      set_float(value.get_float());
    } break;
    case CHARS: {
      set_string(value.get_string().c_str());
    } break;
    case BOOLEANS: {
      set_boolean(value.get_boolean());
    } break;
    case DATES: {
      set_date(value.get_date(), true);
    } break;
    case NULLS: {
      set_null();
    } break;
    case UNDEFINED: {
      ASSERT(false, "got an invalid value type");
    } break;
  }
}

const char *Value::data() const
{
  if (is_null()) return "\n\n\n\n";

  switch (attr_type_) {
    case VAL_LIST: {
      return (const char *)&val_list_value_;
    } break;
    case CHARS: {
      return str_value_.c_str();
    } break;
    default: {
      return (const char *)&num_value_;
    } break;
  }
}

std::string Value::to_string() const
{
  std::stringstream os;
  if (is_null()) return "NULL";
  
  switch (attr_type_) {
    case VAL_LIST: {
      // '(val1, val2, ..., valn)'
    } break;
    case INTS: {
      os << num_value_.int_value_;
    } break;
    case FLOATS: {
      os << common::double_to_str(num_value_.float_value_);
    } break;
    case BOOLEANS: {
      os << num_value_.bool_value_;
    } break;
    case CHARS: {
      std::string str_value = str_value_;
      os << str_value;
    } break;
    case DATES: {
      char date[16];
      snprintf(date, sizeof(date),"%04d-%02d-%02d",num_value_.date_value_/10000,(num_value_.date_value_/100)%100,num_value_.date_value_%100);
      for (int i = 0; i < sizeof(date); i++) {
      if (date[i] == 0) {
        break;
      }
      os << date[i];
      }
    } break;
    default: {
      LOG_WARN("unsupported attr type: %d", attr_type_);
    } break;
  }
  return os.str();
}

bool Value::can_convert(AttrType src_type, AttrType dest_type)
{
  Value src_sample_value, dest;
  src_sample_value.set_type(src_type);
  RC rc = src_sample_value.convert_into(dest_type, dest);
  return rc == RC::SUCCESS;
}

RC Value::convert_into(AttrType type, Value &dest) const
{
  RC rc = RC::SUCCESS;
  if (this->is_null()) {
    dest.set_null();
    return rc;
  }
  if (this->attr_type_ == type) {
    dest.set_value(*this);
    return rc;
  }
  if (this->attr_type_ == INTS && type == FLOATS) {
    dest.set_float((float)this->get_int());
    return rc;
  }
  if (this->attr_type_ == FLOATS && type == INTS) {
    dest.set_int(std::round(this->get_float()));
    return rc;
  }
  if (this->attr_type_ == INTS && type == BOOLEANS) {
    dest.set_boolean(this->get_int() != 0);
  }
  if (this->attr_type_ == FLOATS && type == BOOLEANS) {
    dest.set_boolean(this->get_float() != 0);
  }
  if ((this->attr_type_ == INTS || this->attr_type_ == FLOATS || this->attr_type_ == DATES) &&
      type == CHARS) {
    std::string str = this->to_string();
    dest.set_string(str.c_str(), str.length());
    return rc;
  }
  if (this->attr_type_ == VAL_LIST) {
    dest.set_value(*this);
    while (dest.attr_type() == VAL_LIST) {
      const auto &val_list = dest.get_val_list();
      if (val_list.size() == 0) {
        dest.set_null();
        return rc;
      }
      if (val_list.size() > 1) return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      /*if (val_list.size() > 1) {
        int not_null_cnt = 0;
        for (const auto &val : val_list) {
          if (val.is_null()) continue;
          +not_null_cnt;
        }
      }*/
      dest.set_value(val_list[0]);
    };
    if (dest.attr_type() != type) {
      Value tmp_value;
      rc = dest.convert_into(type, tmp_value);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      dest.set_value(tmp_value);
    }
    return rc;
  }
  if (type == VAL_LIST) {
    dest.set_value(val_list_from(*this));
    return rc;
  }
  return RC::SCHEMA_FIELD_TYPE_MISMATCH;
}

RC Value::compare(const Value &other, Value &result) const
{
  RC rc = RC::SUCCESS;
  if (is_null() || other.is_null()) {
    result.set_null();
    return rc;
  }
  if (this->attr_type_ == other.attr_type_) {
    switch (this->attr_type_) {
      case VAL_LIST: {
        std::vector<Value> this_list = this->get_val_list();
        std::vector<Value> other_list = other.get_val_list();
        if (this_list.size() == 0 || other_list.size() == 0) {
          result.set_null();
          return rc;
        }
        if (this_list.size() != other_list.size()) {
          return RC::SCHEMA_FIELD_TYPE_MISMATCH;
        }
        for (int i = 0; i < this_list.size(); ++i) {
          Value cmp_result;
          rc = this_list[i].compare(other_list[i], cmp_result);
          if (rc != RC::SUCCESS) {
            return rc;
          }
          if (cmp_result.is_null() || cmp_result.get_int() != 0) {
            result.set_value(cmp_result);
            return rc;
          }
        }
        result.set_int(0);
        return rc;
      } break;
      case INTS: {
        result.set_int(common::compare_int((void *)&this->num_value_.int_value_, (void *)&other.num_value_.int_value_));
        return rc;
      } break;
      case FLOATS: {
        result.set_int(common::compare_float((void *)&this->num_value_.float_value_, (void *)&other.num_value_.float_value_));
        return rc;
      } break;
      case CHARS: {
        result.set_int(common::compare_string((void *)this->str_value_.c_str(),
            this->str_value_.length(),
            (void *)other.str_value_.c_str(),
            other.str_value_.length()));
        return rc;
      } break;
      case BOOLEANS: {
        result.set_int(common::compare_int((void *)&this->num_value_.bool_value_, (void *)&other.num_value_.bool_value_));
        return rc;
      } break;
      case DATES: {
        result.set_int(common::compare_int((void *)&this->num_value_.int_value_, (void *)&other.num_value_.int_value_));
        return rc;
      } break; 
      case NULLS: {  //我猜这里不可能出现，毕竟这里的right，left下层已经按照field初始化了attr_type
        return RC::INTERNAL;
      } break;
      default: {
        LOG_WARN("unsupported type: %d", this->attr_type_);
      }
    }
  } else if (this->attr_type_ == INTS && other.attr_type_ == FLOATS) {
    float this_data = this->num_value_.int_value_;
    result.set_int(common::compare_float((void *)&this_data, (void *)&other.num_value_.float_value_));
    return rc;
  } else if (this->attr_type_ == FLOATS && other.attr_type_ == INTS) {
    float other_data = other.num_value_.int_value_;
    result.set_int(common::compare_float((void *)&this->num_value_.float_value_, (void *)&other_data));
    return rc;
  } else if (this->attr_type_ == INTS && other.attr_type_ == CHARS) {
    std::istringstream iss(other.get_string());
    float this_val = this->get_int();
    float other_val = 0;
    iss >> other_val;
    result.set_int(common::compare_float((void *)&this_val, (void *)&other_val));
    return rc;
  } else if (this->attr_type_ == FLOATS && other.attr_type_ == CHARS) {
    std::istringstream iss(other.get_string());
    float this_val = this->get_float();
    float other_val = 0;
    iss >> other_val;
    result.set_int(common::compare_float((void *)&this_val, (void *)&other_val));
    return rc;
  } else if (this->attr_type_ == CHARS && other.attr_type_ == INTS) {
    std::istringstream iss(this->get_string());
    float this_val = 0;
    float other_val = other.get_int();
    iss >> this_val;
    result.set_int(common::compare_float((void *)&this_val, (void *)&other_val));
    return rc;
  } else if (this->attr_type_ == CHARS && other.attr_type_ == FLOATS) {
    std::istringstream iss(this->get_string());
    float this_val = 0;
    float other_val = other.get_float();
    iss >> this_val;
    result.set_int(common::compare_float((void *)&this_val, (void *)&other_val));
    return rc;
  } else if (this->attr_type_ == VAL_LIST) {
    if (this->get_val_list().size() == 0) {
      result.set_null();
      return rc;
    }
    Value cmp_result;
    rc = this->compare(Value::val_list_from(other), cmp_result);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    result.set_value(cmp_result);
    return rc;
  } else if (other.attr_type_ == VAL_LIST) {
    if (other.get_val_list().size() == 0) {
      result.set_null();
      return rc;
    }
    Value cmp_result;
    rc = Value::val_list_from(*this).compare(other, cmp_result);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    result.set_value(cmp_result);
    return rc;
  }
  LOG_WARN("not supported");
  return RC::SCHEMA_FIELD_TYPE_MISMATCH;
}

int Value::get_int() const
{
  if (is_null()) return 0;

  switch (attr_type_) {
    case VAL_LIST: {
      if (val_list_value_.size() == 1) {
        return val_list_value_[0].get_int();
      }
      return 0;
    } break;
    case CHARS: {
      try {
        return (int)(std::stol(str_value_));
      } catch (std::exception const &ex) {
        LOG_TRACE("failed to convert string to number. s=%s, ex=%s", str_value_.c_str(), ex.what());
        return 0;
      }
    }
    case INTS: {
      return num_value_.int_value_;
    }
    case FLOATS: {
      return (int)(num_value_.float_value_);
    }
    case BOOLEANS: {
      return (int)(num_value_.bool_value_);
    }
    case DATES: {
      return (int)(num_value_.date_value_);
    }
    default: {
      LOG_WARN("unknown data type. type=%d", attr_type_);
      return 0;
    }
  }
  return 0;
}

float Value::get_float() const
{
  if (is_null()) return 0;

  switch (attr_type_) {
    case VAL_LIST: {
      if (val_list_value_.size() == 1) {
        return val_list_value_[0].get_float();
      }
      return 0.0;
    } break;
    case CHARS: {
      try {
        return std::stof(str_value_);
      } catch (std::exception const &ex) {
        LOG_TRACE("failed to convert string to float. s=%s, ex=%s", str_value_.c_str(), ex.what());
        return 0.0;
      }
    } break;
    case INTS: {
      return float(num_value_.int_value_);
    } break;
    case FLOATS: {
      return num_value_.float_value_;
    } break;
    case BOOLEANS: {
      return float(num_value_.bool_value_);
    } break;
    case DATES: {
      return float(num_value_.date_value_);
    } break;
    default: {
      LOG_WARN("unknown data type. type=%d", attr_type_);
      return 0;
    }
  }
  return 0;
}

int Value::get_date() const 
{
  if (is_null()) return 0;

  switch (attr_type_)
  {
    case VAL_LIST: {
      if (val_list_value_.size() == 1) {
        return val_list_value_[0].get_date();
      }
      return 0;
    } break;
  case CHARS:{
    try {
      return (int)(std::stol(str_value_));
    } catch (std::exception const &ex) {
      LOG_TRACE("failed to convert string to number. s=%s, ex=%s", str_value_.c_str(), ex.what());
      return 0;
    }
  } break;

  case INTS :{
    return num_value_.int_value_;
  } break;
  
  case FLOATS :{
    return (int)(num_value_.float_value_);
  } break;

  case DATES :{
    return num_value_.date_value_;
  } break;

  default:
    return 0;
  }
}

std::string Value::get_string() const
{
  return this->to_string();
}

bool Value::get_boolean() const
{
  if (is_null()) return false;

  switch (attr_type_) {
    case VAL_LIST: {
      if (val_list_value_.size() == 1) {
        return val_list_value_[0].get_boolean();
      }
      return !val_list_value_.empty();
    } break;
    case CHARS: {
      try {
        float val = std::stof(str_value_);
        if (val >= EPSILON || val <= -EPSILON) {
          return true;
        }

        int int_val = std::stol(str_value_);
        if (int_val != 0) {
          return true;
        }

        return !str_value_.empty();
      } catch (std::exception const &ex) {
        LOG_TRACE("failed to convert string to float or integer. s=%s, ex=%s", str_value_.c_str(), ex.what());
        return !str_value_.empty();
      }
    } break;
    case INTS: {
      return num_value_.int_value_ != 0;
    } break;
    case FLOATS: {
      float val = num_value_.float_value_;
      return val >= EPSILON || val <= -EPSILON;
    } break;
    case BOOLEANS: {
      return num_value_.bool_value_;
    } break;
    case DATES: {
      return num_value_.date_value_ != 0;
    } break;
    case NULLS: {
      return false;
    } break;
    default: {
      LOG_WARN("unknown data type. type=%d", attr_type_);
      return false;
    }
  }
  return false;
}

const std::vector<Value> &Value::get_val_list() const
{
  if (is_null()) return val_list_value_for_null_;

  return val_list_value_;
}
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
// Created by wangyunlai.wyl on 2021/5/19.
//

#include "storage/index/bplus_tree_index.h"
#include "common/log/log.h"

BplusTreeIndex::~BplusTreeIndex() noexcept
{
  close();
}

/*RC BplusTreeIndex::create(const char *file_name, const IndexMeta &index_meta, const FieldMeta &field_meta)
{
  if (inited_) {
    LOG_WARN("Failed to create index due to the index has been created before. file_name:%s, index:%s, field:%s",
        file_name,
        index_meta.name(),
        index_meta.field());
    return RC::RECORD_OPENNED;
  }

  Index::init(index_meta, field_meta);

  RC rc = index_handler_.create(file_name, field_meta.type(), field_meta.len());
  if (RC::SUCCESS != rc) {
    LOG_WARN("Failed to create index_handler, file_name:%s, index:%s, field:%s, rc:%s",
        file_name,
        index_meta.name(),
        index_meta.field(),
        strrc(rc));
    return rc;
  }

  inited_ = true;
  LOG_INFO(
      "Successfully create index, file_name:%s, index:%s, field:%s", file_name, index_meta.name(), index_meta.field());
  return RC::SUCCESS;
}*/

RC BplusTreeIndex::create(const char *file_name, const IndexMeta &index_meta, const FieldMeta &field_meta)
{
  return create(file_name, index_meta, std::vector<const FieldMeta*>{&field_meta});
}

RC BplusTreeIndex::create(const char *file_name, const IndexMeta &index_meta,const std::vector<const FieldMeta *> &field_metas)
{
  if(inited_){
    LOG_WARN("Failed to create index_ due to the index has been created before. file_name:%s, index:%s, field:%s",
             file_name,
             index_meta.name(),
             index_meta.field());
    return RC::RECORD_OPENNED;
  }

  Index::init(index_meta, field_metas);
  std::vector<AttrType> attr_types(field_metas.size());
  std::vector<int> attr_lens(field_metas.size());
  for(int i = 0; i < field_metas.size(); ++i){
    attr_types[i] = field_metas[i]->type();
    attr_lens[i] = field_metas[i]->len();
  }
  RC rc = index_handler_.create(file_name, attr_types, attr_lens);
  if(RC::SUCCESS != rc){
    LOG_WARN("Failed to create index_handler, file_name:%s, index:%s, field:%s, rc:%s",
             file_name,
             index_meta.name(),
             index_meta.field(),
             strrc(rc));
    return rc;
  }

  inited_ = true;
  return rc;
}
/*RC BplusTreeIndex::open(const char *file_name, const IndexMeta &index_meta, const FieldMeta &field_meta)
{
  if (inited_) {
    LOG_WARN("Failed to open index due to the index has been initedd before. file_name:%s, index:%s, field:%s",
        file_name,
        index_meta.name(),
        index_meta.field());
    return RC::RECORD_OPENNED;
  }

  Index::init(index_meta, field_meta);

  RC rc = index_handler_.open(file_name);
  if (RC::SUCCESS != rc) {
    LOG_WARN("Failed to open index_handler, file_name:%s, index:%s, field:%s, rc:%s",
        file_name,
        index_meta.name(),
        index_meta.field(),
        strrc(rc));
    return rc;
  }

  inited_ = true;
  LOG_INFO(
      "Successfully open index, file_name:%s, index:%s, field:%s", file_name, index_meta.name(), index_meta.field());
  return RC::SUCCESS;
}*/

RC BplusTreeIndex::open(const char *file_name, const IndexMeta &index_meta, const FieldMeta &field_meta)
{
  return open(file_name, index_meta, std::vector<const FieldMeta *>{&field_meta});
}

RC BplusTreeIndex::open(const char *file_name, const IndexMeta &index_meta, const std::vector<const FieldMeta *> &field_metas)
{
  if(inited_){
    LOG_WARN("Failed to open index_ due to the index has been initedd before. file_name:%s, index:%s, field:%s",
             file_name,
             index_meta.name(),
             index_meta.field());
    return RC::RECORD_OPENNED;
  }

  Index::init(index_meta, field_metas);

  RC rc = index_handler_.open(file_name);
  if(RC::SUCCESS != rc){
    LOG_WARN("Failed to open index_handler, file_name:%s, index:%s, field:%s, rc:%s",
             file_name,
             index_meta.name(),
             index_meta.field(),
             strrc(rc));
    return rc;
  }
  inited_ = true;
  LOG_INFO("Successfully open index, file_name:%s, index:%s, field:%s", file_name, index_meta.name(), index_meta.field());
  return RC::SUCCESS;
}

RC BplusTreeIndex::close()
{
  if (inited_) {
    LOG_INFO("Begin to close index, index:%s, field:%s", index_meta_.name(), index_meta_.field());
    index_handler_.close();
    inited_ = false;
  }
  LOG_INFO("Successfully close index.");
  return RC::SUCCESS;
}

RC BplusTreeIndex::insert_entry(const char *record, const RID *rid)
{
  //return index_handler_.insert_entry(record + field_meta_.offset(), rid);

  std::vector<const char *> key;   //<----------------------这个offset不太稳定说实话
  for(auto &field_meta : field_meta_){
    key.emplace_back(record + field_meta.offset());
  }
  return index_handler_.insert_entry(key, rid);
}

RC BplusTreeIndex::delete_entry(const char *record, const RID *rid)
{
  //return index_handler_.delete_entry(record + field_meta_.offset(), rid);
  
  std::vector<const char *> key;
  for(auto &field_meta : field_meta_){
    key.emplace_back(record + field_meta.offset());
  }
  return index_handler_.delete_entry(key, rid);
}

IndexScanner *BplusTreeIndex::create_scanner(
    const char *left_key, int left_len, bool left_inclusive, const char *right_key, int right_len, bool right_inclusive)
{
  BplusTreeIndexScanner *index_scanner = new BplusTreeIndexScanner(index_handler_);
  RC rc = index_scanner->open(left_key, left_len, left_inclusive, right_key, right_len, right_inclusive);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open index scanner. rc=%d:%s", rc, strrc(rc));
    delete index_scanner;
    return nullptr;
  }
  return index_scanner;
}

IndexScanner *BplusTreeIndex::create_scanner(std::vector<const char *> left_keys, std::vector<int> left_lens,
                                             bool left_inclusive, std::vector<const char *> right_keys,
                                             std::vector<int> right_lens, bool right_inclusive)
{
  BplusTreeIndexScanner *index_scanner = new BplusTreeIndexScanner(index_handler_);
  RC rc = index_scanner->open(left_keys, left_lens, left_inclusive, right_keys, right_lens, right_inclusive);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open index scanner. rc=%d:%s", rc, strrc(rc));
    delete index_scanner;
    return nullptr;
  }
  return index_scanner;
}

RC BplusTreeIndex::sync()
{
  return index_handler_.sync();
}

////////////////////////////////////////////////////////////////////////////////
BplusTreeIndexScanner::BplusTreeIndexScanner(BplusTreeHandler &tree_handler) : tree_scanner_(tree_handler)
{}

BplusTreeIndexScanner::~BplusTreeIndexScanner() noexcept
{
  tree_scanner_.close();
}

RC BplusTreeIndexScanner::open(
    const char *left_key, int left_len, bool left_inclusive, const char *right_key, int right_len, bool right_inclusive)
{
  return tree_scanner_.open(std::vector<const char *>{left_key}, std::vector<int>{left_len}, left_inclusive,
                            std::vector<const char *>{right_key}, std::vector<int>{right_len}, right_inclusive);
}

RC BplusTreeIndexScanner::open(std::vector<const char *> left_keys, std::vector<int> left_lens, bool left_inclusive,
                          std::vector<const char *> right_keys, std::vector<int> right_lens, bool right_inclusive)
{
  return tree_scanner_.open(left_keys, left_lens, left_inclusive, right_keys, right_lens, right_inclusive);
}

RC BplusTreeIndexScanner::next_entry(RID *rid)
{
  return tree_scanner_.next_entry(*rid);
}

RC BplusTreeIndexScanner::destroy()
{
  delete this;
  return RC::SUCCESS;
}

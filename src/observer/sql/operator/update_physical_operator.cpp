#include "common/log/log.h"
#include "sql/operator/update_physical_operator.h"
#include "storage/record/record.h"
#include "storage/table/table.h"
#include "storage/trx/trx.h"
#include "sql/stmt/update_stmt.h"

RC UpdatePhysicalOperator::open(Trx *trx)
{
    if (children_.empty()) {
        return RC::SUCCESS;
    }

    std::unique_ptr<PhysicalOperator> &child = children_[0];
    RC rc = child->open(trx);
    if (rc != RC::SUCCESS) {
        LOG_WARN("failed to open child operator: %s", strrc(rc));
        return rc;
    }

    trx_ = trx;

    return RC::SUCCESS;
}

RC UpdatePhysicalOperator::next()
{
    RC rc = RC::SUCCESS;
    if (children_.empty()) {
        return RC::RECORD_EOF;
    }

    PhysicalOperator *child = children_[0].get();
    while (RC::SUCCESS == (rc = child->next())) {
        Tuple *tuple = child->current_tuple();
        if (nullptr == tuple) {
            LOG_WARN("failed to get current record: %s", strrc(rc));
            return rc;
        }

        TableAliasTuple *table_alias_tuple = static_cast<TableAliasTuple *>(tuple);
        RowTuple *row_tuple = static_cast<RowTuple *>(table_alias_tuple->tuple());
        Record &record = row_tuple->record();
        for (auto &item : update_items_){
            if (item.value.is_null() &&
                !item.field->meta()->nullable()){
                LOG_WARN("bad attempt to assign NULL value to a non-nullable field");
                return RC::SCHEMA_FIELD_TYPE_MISMATCH;
            }

            // 如果类型不匹配，尝试转换值类型（例如：子查询返回的结果集 VAL_LIST 类型，如果只有一条结果，且只有一列，则可以转换成单值）
            AttrType type = item.field->meta()->type();
            if (item.value.attr_type() != type) {
                Value converted_value;
                rc = item.value.convert_into(type, converted_value);
                if (rc != RC::SUCCESS) {
                    LOG_WARN("failed to convert value type when creating update stmt");
                    return rc;
                }
                if (converted_value.is_null() && !item.field->meta()->nullable()) {
                    LOG_WARN("bad attempt to assign NULL value to a non-nullable field");
                    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
                }
                item.value.set_value(converted_value);
            }
        }
        rc= trx_->update_record(table_, record, update_items_);
        if (rc != RC::SUCCESS) {
            LOG_WARN("failed to update record: %s", strrc(rc));
            return rc;
        }
    }

    return RC::RECORD_EOF;
}


RC UpdatePhysicalOperator::close()
{
    if (!children_.empty()) {
        children_[0]->close();
    }
    return RC::SUCCESS;
}
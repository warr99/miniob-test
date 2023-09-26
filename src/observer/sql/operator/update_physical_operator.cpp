#include "sql/operator/update_physical_operator.h"
#include "sql/stmt/update_stmt.h"
#include "storage/table/table.h"
#include "storage/trx/trx.h"

using namespace std;

RC UpdatePhysicalOperator::open(Trx* trx) {
    if (children_.empty()) {
        return RC::SUCCESS;
    }

    std::unique_ptr<PhysicalOperator>& child = children_[0];
    RC rc = child->open(trx);
    if (rc != RC::SUCCESS) {
        LOG_WARN("failed to open child operator: %s", strrc(rc));
        return rc;
    }

    trx_ = trx;

    return RC::SUCCESS;
}

RC UpdatePhysicalOperator::next() {
    RC rc = RC::SUCCESS;
    if (children_.empty()) {
        return RC::RECORD_EOF;
    }

    PhysicalOperator* child = children_[0].get();
    while (RC::SUCCESS == (rc = child->next())) {
        Tuple* tuple = child->current_tuple();
        if (nullptr == tuple) {
            LOG_WARN("failed to get current record: %s", strrc(rc));
            return rc;
        }

        RowTuple* row_tuple = static_cast<RowTuple*>(tuple);
        Record& record = row_tuple->record();
        if (*field_name_ == 0) {
            rc = RC::EMPTY;
            return rc;
        }
        const FieldMeta* feildmeta = table_->table_meta().field(field_name_);
        if (feildmeta == nullptr)
        {
            rc = RC::EMPTY;
            return rc;
        }
        

        int offset = feildmeta->offset();
        int len = feildmeta->len();

        rc = trx_->update_record(table_, record, value_, offset,len);
        if (rc != RC::SUCCESS) {
            LOG_WARN("failed to delete record: %s", strrc(rc));
            return rc;
        }
    }
    return RC::RECORD_EOF;
}

RC UpdatePhysicalOperator::close() {
    if (!children_.empty()) {
        children_[0]->close();
    }
    return RC::SUCCESS;
}
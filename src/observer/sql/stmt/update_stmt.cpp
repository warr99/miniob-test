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

#include "sql/stmt/update_stmt.h"
#include <string>
#include "common/log/log.h"
#include "common/util/date.h"
#include "sql/stmt/filter_stmt.h"
#include "storage/db/db.h"
#include "storage/table/table.h"

UpdateStmt::UpdateStmt(Table* table, Value* values, int value_amount, FilterStmt* filter_stmt, const std::string& attribute_name)
    : table_(table), values_(values), value_amount_(value_amount), filter_stmt_(filter_stmt), attribute_name_(attribute_name) {}

RC UpdateStmt::create(Db* db, const UpdateSqlNode& update, Stmt*& stmt) {

    const char* table_name = update.relation_name.c_str();
    if (nullptr == db || nullptr == table_name || update.attribute_name.empty()) {
        LOG_WARN("invalid argument. db=%p, table_name=%p", db, table_name);
        return RC::INVALID_ARGUMENT;
    }

    // 检查表格是否存在
    Table* table = db->find_table(table_name);
    if (nullptr == table) {
        LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    }

    // 生成过滤条件
    std::unordered_map<std::string, Table*> table_map;
    table_map.insert(std::pair<std::string, Table*>(std::string(table_name), table));
    FilterStmt* filter_stmt = nullptr;
    RC rc = FilterStmt::create(
        db, table, &table_map, update.conditions.data(), static_cast<int>(update.conditions.size()), filter_stmt);
    if (rc != RC::SUCCESS) {
        LOG_WARN("failed to create filter statement. rc=%d:%s", rc, strrc(rc));
        return rc;
    }
    Value &value = const_cast<Value &>(update.value);
    const TableMeta& table_meta = table->table_meta();

    // 检查插入的字段
    const std::string attribute_name = update.attribute_name;
    const int sys_field_num = table_meta.sys_field_num();
    const int field_num = table_meta.field_num();

    for (int i = 0; i < field_num; i++) {
        const FieldMeta* field_meta = table_meta.field(i + sys_field_num);
        // 找到要更新的字段
        if (field_meta->name() == attribute_name) {
            const AttrType field_type = field_meta->type();
            const AttrType value_type = value.attr_type();
            if (field_type != value_type) {
                if (field_type == DATES) {
                    int date = -1;
                    // 把 string 表示的 date 转化成 int
                    RC rc = string_to_date(value.data(), date);
                    if (rc != RC::SUCCESS) {
                        LOG_TRACE("transform to date fail,%d", value.data());
                        return rc;
                    }
                    value.set_date(date);
                } else if (field_type == TEXTS && value_type == CHARS) {
                    value.set_text(value.data());
                } else {
                    LOG_WARN("field type mismatch. table=%s, field=%s, field type=%d, value_type=%d",
                             table_name, field_meta->name(), field_type, value_type);
                    return RC::SCHEMA_FIELD_TYPE_MISMATCH;
                }
            }
            // 生成 stmt
            stmt = new UpdateStmt(table, &value, 1, filter_stmt, attribute_name);
            return RC::SUCCESS;
        }
    }
    return RC::INTERNAL;
}

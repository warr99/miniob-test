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
// Created by Wangyunlai on 2022/6/6.
//

#include "sql/stmt/select_stmt.h"
#include "common/lang/string.h"
#include "common/log/log.h"
#include "sql/fun/max_func.h"
#include "sql/fun/min_func.h"
#include "sql/fun/count_func.h"
#include "sql/fun/avg_func.h"
#include "sql/stmt/filter_stmt.h"
#include "storage/db/db.h"
#include "storage/table/table.h"

SelectStmt::~SelectStmt() {
    if (nullptr != filter_stmt_) {
        delete filter_stmt_;
        filter_stmt_ = nullptr;
    }
}

AggregationFunc* create_func(const char* func_name) {
    FuncType type = stringToFuncType(func_name);
    switch (type) {
        case FuncType::FUNC_MAX: {
            return new MaxFunc();
        }
        case FuncType::FUNC_MIN: {
            return new MinFunc();
            break;
        }
        case FuncType::FUNC_COUNT: {
            return new CountFunc();
            break;
        }
        case FuncType::FUNC_AVG: {
            return new AvgFunc();
            break;
        }
        default:
            return new AggregationFunc();
    }
    return new AggregationFunc();
}

static void wildcard_fields(Table* table, std::vector<Field>& field_metas, const char* func_name) {
    const TableMeta& table_meta = table->table_meta();
    const int field_num = table_meta.field_num();
    for (int i = table_meta.sys_field_num(); i < field_num; i++) {
        AggregationFunc* func = create_func(func_name);
        field_metas.push_back(Field(table, table_meta.field(i), func));
        // delete func;
    }
}

RC SelectStmt::create(Db* db, const SelectSqlNode& select_sql, Stmt*& stmt) {
    if (nullptr == db) {
        LOG_WARN("invalid argument. db is null");
        return RC::INVALID_ARGUMENT;
    }

    // collect tables in `from` statement 从 from 里面收集表名
    std::vector<Table*> tables;
    std::unordered_map<std::string, Table*> table_map;
    for (size_t i = 0; i < select_sql.relations.size(); i++) {
        const char* table_name = select_sql.relations[i].c_str();
        if (nullptr == table_name) {
            LOG_WARN("invalid argument. relation name is null. index=%d", i);
            return RC::INVALID_ARGUMENT;
        }

        // 判断表是否存在
        Table* table = db->find_table(table_name);
        if (nullptr == table) {
            // 不存在返回错误
            LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
            return RC::SCHEMA_TABLE_NOT_EXIST;
        }

        tables.push_back(table);
        table_map.insert(std::pair<std::string, Table*>(table_name, table));
    }

    // collect query fields in `select` statement 从 select 里面收集字段名
    std::vector<Field> query_fields;
    for (int i = static_cast<int>(select_sql.attributes.size()) - 1; i >= 0; i--) {
        const RelAttrSqlNode& relation_attr = select_sql.attributes[i];

        // 将 "*" 展开成所有表字段
        if (common::is_blank(relation_attr.relation_name.c_str()) &&
            0 == strcmp(relation_attr.attribute_name.c_str(), "*")) {
            for (Table* table : tables) {
                wildcard_fields(table, query_fields, relation_attr.aggregation_func.c_str());
            }

        } else if (!common::is_blank(relation_attr.relation_name.c_str())) {
            const char* table_name = relation_attr.relation_name.c_str();
            const char* field_name = relation_attr.attribute_name.c_str();
            const char* func_name = relation_attr.aggregation_func.c_str();

            if (0 == strcmp(table_name, "*")) {
                if (0 != strcmp(field_name, "*")) {
                    LOG_WARN("invalid field name while table is *. attr=%s", field_name);
                    return RC::SCHEMA_FIELD_MISSING;
                }
                for (Table* table : tables) {
                    wildcard_fields(table, query_fields, func_name);
                }
            } else {
                auto iter = table_map.find(table_name);
                if (iter == table_map.end()) {
                    LOG_WARN("no such table in from list: %s", table_name);
                    return RC::SCHEMA_FIELD_MISSING;
                }

                Table* table = iter->second;
                if (0 == strcmp(field_name, "*")) {
                    wildcard_fields(table, query_fields, func_name);
                } else {
                    const FieldMeta* field_meta = table->table_meta().field(field_name);
                    if (nullptr == field_meta) {
                        LOG_WARN("no such field. field=%s.%s.%s", db->name(), table->name(), field_name);
                        return RC::SCHEMA_FIELD_MISSING;
                    }
                    AggregationFunc* func = create_func(func_name);
                    query_fields.push_back(Field(table, field_meta, func));
                    delete func;
                }
            }
        } else {
            // 暂时不接受表超过一个的情况
            if (tables.size() != 1) {
                LOG_WARN("invalid. I do not know the attr's table. attr=%s", relation_attr.attribute_name.c_str());
                return RC::SCHEMA_FIELD_MISSING;
            }

            Table* table = tables[0];
            const FieldMeta* field_meta = table->table_meta().field(relation_attr.attribute_name.c_str());
            if (nullptr == field_meta) {
                LOG_WARN("no such field. field=%s.%s.%s", db->name(), table->name(), relation_attr.attribute_name.c_str());
                return RC::SCHEMA_FIELD_MISSING;
            }
            AggregationFunc* func = create_func(relation_attr.aggregation_func.c_str());
            query_fields.push_back(Field(table, field_meta, func));
            // delete func;
        }
    }

    LOG_INFO("got %d tables in from stmt and %d fields in query stmt", tables.size(), query_fields.size());

    Table* default_table = nullptr;
    if (tables.size() == 1) {
        default_table = tables[0];
    }

    // create filter statement in `where` statement
    FilterStmt* filter_stmt = nullptr;
    RC rc = FilterStmt::create(db,
                               default_table,
                               &table_map,
                               select_sql.conditions.data(),
                               static_cast<int>(select_sql.conditions.size()),
                               filter_stmt);
    if (rc != RC::SUCCESS) {
        LOG_WARN("cannot construct filter stmt");
        return rc;
    }

    // everything alright
    SelectStmt* select_stmt = new SelectStmt();
    // TODO add expression copy
    select_stmt->tables_.swap(tables);
    select_stmt->query_fields_.swap(query_fields);
    select_stmt->filter_stmt_ = filter_stmt;
    stmt = select_stmt;
    return RC::SUCCESS;
}

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
// Created by Wangyunlai on 2023/6/13.
//

#include "sql/stmt/create_table_stmt.h"
#include "common/log/log.h"
#include "common/util/date.h"
#include "event/sql_debug.h"
#include "sql/stmt/insert_stmt.h"
#include "storage/db/db.h"
#include "storage/table/table.h"

RC CreateTableStmt::create(Db* db, const CreateTableSqlNode& create_table, Stmt*& stmt) {
    const char* table_name = create_table.relation_name.c_str();
    if (nullptr == db || nullptr == table_name || create_table.attr_infos.empty()) {
        return RC::INVALID_ARGUMENT;
    }
    // 检查表格是否已经存在
    Table* table = db->find_table(table_name);
    if (nullptr != table) {
        LOG_WARN("Table already exists. db=%s, table_name=%s", db->name(), table_name);
        return RC::SCHEMA_TABLE_NOT_EXIST;
    }
    stmt = new CreateTableStmt(create_table.relation_name, create_table.attr_infos);
    sql_debug("create table statement: table name %s", create_table.relation_name.c_str());
    return RC::SUCCESS;
}

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
#include "common/lang/string.h"
#include "common/log/log.h"
#include "json/json.h"
#include "storage/field/field_meta.h"
#include "storage/table/table_meta.h"
#include <vector>

const static Json::StaticString FIELD_NAME("name");
const static Json::StaticString FIELD_FIELD_NAME("field_name");
const static Json::StaticString INDEX_TYPE("index_type");

RC IndexMeta::init(const char* name, const std::vector<FieldMeta> fields, IndexType index_type) {
    if (common::is_blank(name)) {
        LOG_ERROR("Failed to init index, name is empty.");
        return RC::INVALID_ARGUMENT;
    }

    name_ = name;
    std::vector<std::string> field_names;
    for (int i = 0; i < fields.size(); i++) {
        field_names.push_back(fields[i].name());
    }
    field_names_ = field_names;
    index_type_ = index_type;
    return RC::SUCCESS;
}

void IndexMeta::to_json(Json::Value& json_value) const {
    Json::Value json_array(Json::arrayValue);
    // 遍历field_names并将其添加到JSON数组中
    for (const std::string& field_name : field_names_) {
        json_array.append(field_name);
    }
    // 将Json::Value对象转换为JSON字符串
    Json::StreamWriterBuilder writer;
    std::string field_names_json_str = Json::writeString(writer, json_array);

    json_value[FIELD_NAME] = name_;
    json_value[FIELD_FIELD_NAME] = field_names_json_str;
    json_value[INDEX_TYPE] = indexToString(index_type_);
}

RC IndexMeta::from_json(const TableMeta& table, const Json::Value& json_value, IndexMeta& index) {
    const Json::Value& name_value = json_value[FIELD_NAME];
    const Json::Value& field_value_list = json_value[FIELD_FIELD_NAME];
    const Json::Value& index_type_value = json_value[INDEX_TYPE];
    if (!name_value.isString()) {
        LOG_ERROR("Index name is not a string. json value=%s", name_value.toStyledString().c_str());
        return RC::INTERNAL;
    }

    if (!field_value_list.isString()) {
        LOG_ERROR("Field name of index [%s] is not a string. json value=%s",
                  name_value.asCString(),
                  field_value_list.toStyledString().c_str());
        return RC::INTERNAL;
    }

    if (!index_type_value.isString()) {
        LOG_ERROR("Field name of index [%s] is not a string. json value=%s",
                  name_value.asCString(),
                  field_value_list.toStyledString().c_str());
        return RC::INTERNAL;
    }
    // 将 field_value_list 转化成 std::vector<std::string>
    std::vector<std::string> field_name_list = jsonToList(field_value_list);
    std::vector<FieldMeta> field_list;
    for (int i = 0; i < field_name_list.size(); i++) {
        const FieldMeta* fieldMeta = table.field(field_name_list[i].c_str());
        if (fieldMeta == nullptr) {
            {
                LOG_ERROR("Deserialize index [%s]: no such field: %s", name_value.asCString(), field_name_list[i].c_str());
                return RC::SCHEMA_FIELD_MISSING;
            }
        }
        field_list.push_back(*fieldMeta);
    }
    IndexType index_type = stringToIndex(index_type_value.asCString());

    return index.init(name_value.asCString(), field_list, index_type);
}

const char* IndexMeta::name() const {
    return name_.c_str();
}

const std::vector<std::string> IndexMeta::field() const {
    return field_names_;
}

const IndexType IndexMeta::indexType() const {
    return index_type_;
}

void IndexMeta::desc(std::ostream& os) const {
    // TODO 输出
    os << "index name=" << name_ << ", field=" << field_names_[0];
}

std::vector<std::string> jsonToList(const Json::Value& json_array) {
    std::vector<std::string> result;

    if (!json_array.isArray()) {
        std::cerr << "Input JSON is not an array." << std::endl;
        return result;  // 返回空vector
    }

    for (const Json::Value& item : json_array) {
        if (item.isString()) {
            result.push_back(item.asString());
        } else {
            std::cerr << "Non-string item found in the JSON array." << std::endl;
        }
    }

    return result;
}
#pragma once

#include <stdint.h>
#include <sstream>
#include <string>
#include "common/rc.h"
#include "sql/parser/parse_defs.h"

inline bool is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

inline bool is_valid_date(int year, int month, int day) {
    if (year < 1900 || year > 9999 ||
        month <= 0 || month > 12 ||
        day <= 0 || day > 31) {
        return false;
    }
    int max_day_in_month[] = {
        31,
        29,
        31,
        30,
        31,
        30,
        31,
        31,
        30,
        31,
        30,
        31,
    };
    const int max_day = max_day_in_month[month - 1];
    if (day > max_day) {
        return false;
    }
    if (month == 2 && !is_leap_year(year) && day > 28) {
        return false;
    }
    return true;
}

inline RC string_to_date(const char* str, int& date) {
    int year = 0;
    int month = 0;
    int day = 0;
    int ret = sscanf(str, "%d-%d-%d", &year, &month, &day);
    if (ret != 3) {
        return RC::INVALID_ARGUMENT;
    }
    if (!is_valid_date(year, month, day)) {
        return RC::INVALID_ARGUMENT;
    }
    date = year * 10000 + month * 100 + day;
    return RC::SUCCESS;
}

inline RC date_to_string(int date, char* str) {
    int year = date / 10000;
    int month = (date / 100) % 100;
    int day = date % 100;
    if (!is_valid_date(year, month, day)) {
        return RC::INVALID_ARGUMENT;
    }
    sprintf(str, "%04d-%02d-%02d", year, month, day);
}

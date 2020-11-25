//
// Created by yinxin on 2020/10/5.
//

#include <mysql/mysql.h>
#include "nlohmann/json.hpp"

#ifndef JSON2SQL_TOOL_UTIL_H
#define JSON2SQL_TOOL_UTIL_H

#endif //JSON2SQL_TOOL_UTIL_H

MYSQL_RES * makeMysqlRes(MYSQL *conn, const nlohmann::json& fieldRoot, const nlohmann::json& rowsRoot);
void makeMysqlField(MYSQL_FIELD *tmp, nlohmann::json field);
MYSQL_FIELD* makeMysqlFields(nlohmann::json fields);
MYSQL_ROWS *makeMysqlRows(nlohmann::json rows);
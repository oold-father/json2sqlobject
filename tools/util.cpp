//
// Created by yinxin on 2020/10/5.
//
#include <string>
#include <json/json.h>
#include <mysql/mysql.h>
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using string = std::string;

void EraseChar(string &s, char &c){
    s.erase(s.find_last_not_of(c) + 1);
    s.erase(0, s.find_first_not_of(c));
}

void makeMysqlField(MYSQL_FIELD *tmp, nlohmann::json field) {
    /*
     tmp: 外部初始化的MYSQL_FIELD指针
     field: json对象，结构如下
        {
            "name": "age",
            "org_name": "age",
            "table": "json",
            "org_table": "json",
            "db": "test",
            "type": "int"
         }
     * */
    const char *temp;
    string s;
    if (field == nullptr) {
        return;
    }
    s = field["name"].dump();
    EraseChar(s, (char &) "\"");
    temp = s.c_str();
    tmp->name = strdup(temp);


    s = field["org_name"].dump();
    EraseChar(s, (char &) "\"");
    temp = s.c_str();
    tmp->org_name = strdup(temp);;

    s = field["table"].dump();
    EraseChar(s, (char &) "\"");
    temp = s.c_str();
    tmp->table = strdup(temp);;

    s = field["org_table"].dump();
    EraseChar(s, (char &) "\"");
    temp = s.c_str();
    tmp->org_table = strdup(temp);;

    s = field["db"].dump();
    EraseChar(s, (char &) "\"");
    temp = s.c_str();
    tmp->db = strdup(temp);
    tmp->catalog = (char *) &"def";
    tmp->def = nullptr;

    tmp->max_length = 0;
    tmp->name_length = strlen(tmp->name);
    tmp->org_name_length = strlen(tmp->org_name);
    tmp->table_length = strlen(tmp->table);
    tmp->org_table_length = strlen(tmp->org_table);
    tmp->db_length = strlen(tmp->db);
    tmp->catalog_length = strlen(tmp->catalog);
    tmp->def_length = 0;
    tmp->flags = 0;
    tmp->decimals = 0;
    tmp->charsetnr = 63;

    if (strcmp(field["type"].dump().c_str(), "int") == 0) {
        tmp->type = MYSQL_TYPE_LONGLONG;
        tmp->length = 11;
    } else if (strcmp(field["type"].dump().c_str(), "float") == 0) {
        tmp->type = MYSQL_TYPE_FLOAT;
        tmp->length = 11;
    } else {
        tmp->type = MYSQL_TYPE_STRING;
        tmp->length = 1020;
    }
    tmp->extension = nullptr;
}

MYSQL_FIELD *makeMysqlFields(MYSQL_FIELD *result, nlohmann::json fields) {
    /*
     tmp: 外部初始化的MYSQL_FIELD指针
     fields: json对象，结构如下
        [{
            "name": "age",
            "org_name": "age",
            "table": "json",
            "org_table": "json",
            "db": "test",
            "type": "int"
         },
         {
            "name": "name",
            "org_name": "name",
            "table": "json",
            "org_table": "json",
            "db": "test",
            "type": "string"
         }]
     * */
    MYSQL_FIELD *tmp;
    int i = 0;
    int count;

    count = fields.size();

    if (fields == nullptr || count < 1) {
        return nullptr;
    }

    for (const auto &item : fields.items()) {
        tmp = result + i;
        makeMysqlField(tmp, item.value());
        i++;
    }

    return result;
}

MYSQL_ROWS *makeMysqlRows(nlohmann::json rows) {
    /*
     rows: json对象,其结构和makeMysqlFields的入参fields所指定结构的一致，
     以makeMysqlFields的示例结构为例，应当输入的rows结构如下：
     [{"name":"yinxin","age":22},
     {"name":"geek","age":21}]
     * */
    MYSQL_ROW mysqlRow;
    MYSQL_ROWS *mysqlRows;
    MYSQL_ROWS *parent;
    MYSQL_ROWS *result = nullptr;
    int fieldCount;
    int i;
    const char *valueStr;

    for (const auto &row : rows.items()) {
        mysqlRows = (MYSQL_ROWS *) malloc(sizeof(MYSQL_ROWS));
        memset(mysqlRows, 0, sizeof(MYSQL_ROWS));

        fieldCount = row.value().size();
        mysqlRow = (MYSQL_ROW) malloc(sizeof(char **) * fieldCount);
        memset(mysqlRow, 0, sizeof(char **) * fieldCount);

        i = 0;
        for (const auto &item : row.value().items()) {
            string s = item.value().dump();
            EraseChar(s, (char &) "\"");
            valueStr = s.c_str();
            if (strcmp(valueStr, "null") == 0){
                mysqlRow[i] = nullptr;
            } else{
                mysqlRow[i] = (char *) malloc(strlen((char *) valueStr) + 1);
                strcpy(mysqlRow[i], (char *) valueStr);
            }
            i++;
        }
        mysqlRows->data = mysqlRow;

        if (result == nullptr) {
            parent = result = mysqlRows;
        } else {
            parent->next = mysqlRows;
            parent = parent->next;
        }
    }
    return result;
}

MYSQL_RES *makeMysqlRes(MYSQL *conn, const nlohmann::json &fieldRoot, const nlohmann::json &rowsRoot) {
    /*
     * conn: MYSQL连接的结构体
     * fieldRoot: json对象的地址，结构参见makeMysqlFields入参
     * rowsRoot: json对象的地址，makeMysqlRows
     * */
    MYSQL_RES *myRes;
    unsigned long *lengths;
    unsigned long lenTemp = 0;
    unsigned long memSize;
    MYSQL_FIELD *mysqlField;
    MYSQL_DATA *mysqlData;

    lengths = &lenTemp;

    myRes = (MYSQL_RES *) malloc(sizeof(MYSQL_RES));
    // default 0
    myRes->row_count = 0;
    myRes->current_field = 0;
    myRes->current_row = nullptr;
    myRes->extension = nullptr;
    // default bool
    myRes->eof = false;
    myRes->unbuffered_fetch_cancelled = false;
    // default ptr. value 0
    myRes->lengths = lengths;
    // object of conn
    myRes->handle = conn;
    myRes->methods = conn->methods;
    // function ptr of conn
    myRes->field_alloc = conn->field_alloc;
    // field of conn
    myRes->metadata = conn->resultset_metadata;

    // default nullptr
    mysqlData = (MYSQL_DATA *) malloc(sizeof(MYSQL_DATA));
    memset(mysqlData, 0, sizeof(MYSQL_DATA));
    myRes->data = mysqlData;
    /*******************************************
     *  需要给的数据
     *******************************************/
    // data fields
    memSize = sizeof(MYSQL_FIELD) * fieldRoot.size();
    mysqlField = (MYSQL_FIELD *) malloc(memSize);
    memset(mysqlField, 0, memSize);
    myRes->fields = makeMysqlFields(mysqlField, fieldRoot);
    myRes->field_count = fieldRoot.size();

    // data rows
    myRes->data_cursor = makeMysqlRows(rowsRoot);
    return myRes;
}
//
// Created by yinxin on 2020/10/5.
//
#include <iostream>
#include <string>
#include <json/json.h>
#include <mysql/mysql.h>
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using string = std::string;

void JsonTest() {

    std::cout << "input json:" << std::endl;
    string strValue = R"({"name":"yinxin","age":22})";
    json root = json::parse(strValue);

    std::cout << root.dump(4) << std::endl;
}

MYSQL_FIELD *fieldTest(MYSQL_FIELD *mysqlField, int len){
    MYSQL_FIELD *tmp;
    int i;
    char **a;
    char b[] = "age";
    char c[] = "name";
    a = (char **)malloc(sizeof(char **)*2);

    a[0]= (char *)malloc(strlen((char *)b) + 1);
    strcpy(a[0], (char *)b);

    a[1]= (char *)malloc(strlen((char *)c) + 1);
    strcpy(a[1], (char *)c);

    for (i=0;i<len;i++) {
        tmp = mysqlField + i;
        tmp->name = a[i];
        tmp->org_name = a[i];
        tmp->table = (char *) &"json";
        tmp->org_table = (char *) &"josn";
        tmp->db = (char *) &"test";
        tmp->catalog = (char *) &"def";
        tmp->def = nullptr;
        tmp->length = 11;
        tmp->max_length = 0;
        tmp->name_length = strlen(mysqlField->name);
        tmp->org_name_length = strlen(mysqlField->org_name);
        tmp->table_length = strlen(mysqlField->table);
        tmp->org_table_length = strlen(mysqlField->org_table);
        tmp->db_length = strlen(mysqlField->db);
        tmp->catalog_length = strlen(mysqlField->catalog);
        tmp->def_length = 0;
        tmp->flags = 49699;
        tmp->decimals= 0;
        tmp->charsetnr=63;
        // !!!告知需要类型
        tmp->type=MYSQL_TYPE_LONGLONG;
        tmp->extension= nullptr;
    }

    return mysqlField;
}

void TestRows(MYSQL_ROWS* mysqlRows){

    MYSQL_ROW row;
    row = (MYSQL_ROW)malloc(sizeof(char **)*2);
    memset(row, 0, sizeof(char **)*2);
    char a[]= "22";
    char b[]= "yinxin";
    row[0]= (char *)malloc(strlen((char *)a) + 1);
    strcpy(row[0], (char *)a);

    row[1]= (char *)malloc(strlen((char *)b) + 1);
    strcpy(row[1], (char *)b);

    mysqlRows->data = row;
}

MYSQL_RES *ResTest(MYSQL *conn) {
    MYSQL_RES *myRes;
    MYSQL_ROWS *mysqlRows;
    unsigned long *lengths;
    unsigned long lenTemp = 0;
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
    /*******************************************
     * data
     *******************************************/
    // data fields

    mysqlField = (MYSQL_FIELD *) malloc(sizeof(MYSQL_FIELD)*2);
    myRes->fields = fieldTest(mysqlField, 2);
    myRes->field_count = 2;

    // default nullptr
    mysqlData = (MYSQL_DATA*) malloc(sizeof(MYSQL_DATA));
    memset(mysqlData, 0, sizeof(MYSQL_DATA));
    myRes->data = mysqlData;

    mysqlRows = (MYSQL_ROWS *)malloc(sizeof(MYSQL_ROWS));
    memset(mysqlRows, 0, sizeof(MYSQL_ROWS));
    TestRows(mysqlRows);
    myRes->data_cursor = mysqlRows;
    return myRes;
}



void MyTest() {
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;
    MYSQL_FIELD *field;
    unsigned int num_fields;
    unsigned int i;

    char server[] = "localhost";
    char user[] = "root";
    char password[] = "geek";
    char database[] = "test";

    conn = mysql_init(nullptr);

    if (!mysql_real_connect(conn, server, user, password, database, 0, nullptr, 0)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    // select * from test.json;
    const char *sql = "select * from json;";
    if (mysql_real_query(conn, sql, strlen(sql))) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    res = ResTest(conn);

    printf("Result:\n");

    printf("**************\n");
    while ((field = mysql_fetch_field(res)))
    {
        printf("%s\t", field->name);
    }
    printf("\n**************\n");

    num_fields = mysql_num_fields(res);
    while ((row = mysql_fetch_row(res)))
    {
        unsigned long *lengths;
        lengths = mysql_fetch_lengths(res);
        for(i = 0; i < num_fields; i++)
        {
            printf("%s",row[i] ? row[i] : "NULL");
            printf("\t");
        }
        printf("\n");
    }
    printf("**************\n");


//    mysql_free_result(res);
    mysql_close(conn);
}


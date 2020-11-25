#include <iostream>
#include <string>
#include <cstring>
#include "tools/util.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;
using string = std::string;

int main() {
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


    std::cout << "input field json:" << std::endl;
    string fieldStr = R"([{
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
     }])";
    json fieldRoot = json::parse(fieldStr);
    std::cout << fieldRoot.dump(4) << std::endl;

    std::cout << "input row json:" << std::endl;
    string rowsValue = R"([{"name":"yinxin","age":22},{"name":"geek","age":21}])";
    json rowsRoot = json::parse(rowsValue);
    std::cout << rowsRoot.dump(4) << std::endl;
    std::cout << "\r" << std::endl;

    conn = mysql_init(nullptr);

    if (!mysql_real_connect(conn, server, user, password, database, 0, nullptr, 0)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    // 调用自定义的函数生成查询结果
    // select * from test.json;
    res = makeMysqlRes(conn, fieldRoot, rowsRoot);

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

    // 调用该函数可能会有释放两次内存的问题
    // mysql_free_result(res);
    mysql_close(conn);

    return 0;
}


# json2sqlobject
从Json转换到C查询结果的Object

## 概述
通常我们使用mysql驱动的函数

    MYSQL_RES *mysql_use_result(MYSQL *conn)
    
来获取到 `MYSQL_RES` 的结构体

现在可以通过提供 `MYSQL` 结构体、两个 `json` 结构来模拟真实请求，生成`MYSQL_RES` 的结构体

    MYSQL_RES *makeRes(MYSQL *conn,const nlohmann::json& fieldRoot,const nlohmann::json& rowsRoot);

`fieldRoot` 的结构

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
    },
    '''
    ]

`rowsRoot` 的结构
 
    [{
        "name":"yinxin",
        "age":22
    },
    {
        "name":"geek",
        "age":21
    },
    ''''
    ]
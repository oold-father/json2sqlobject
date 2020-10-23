cmake_minimum_required(VERSION 3.17)
project(json2sql_tool)

set(CMAKE_CXX_STANDARD 14)
set(EXE_NAME "json2sql_tool")

#jsoncpp
set(jsoncpp_include_dir "/usr/local/include")
set(jsoncpp_link_dir "/usr/lib/x86_64-linux-gnu")
include_directories(${jsoncpp_include_dir})
link_directories(${jsoncpp_link_dir})


add_executable(json2sql_tool main.cpp tools/util.h tools/util.cpp tools/my_structs.h)

target_link_libraries(${EXE_NAME} jsoncpp dl mysqlclient)
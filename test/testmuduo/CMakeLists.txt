#配置头文件搜索路径
#include_directories()
#配置库文件搜索路径
#link_libraries()

#设置需要编译的源文件列表
#set(SRC_LIST muduo_server.cpp)
#生成可执行文件
#add_executable(server muduo_server.cpp)
#add_executable(server SRC_LIST)

#把 .制定路径下的所有源文件放进SRC
# aux_source_directory(. SRC_LIST)

#设置可执行文件存储的路径
#
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(server muduo_server.cpp)
# server 需要链接的库
target_link_libraries(server muduo_net muduo_base pthread)
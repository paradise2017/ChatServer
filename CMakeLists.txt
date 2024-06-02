#入口的Cmakelists
cmake_minimum_required(VERSION 3.0)
project(chat)
#配置编译选项  在CMAKE_CXX_FLAGS基础上加上-g选项 可以调试
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

#配置最终的可执行文件输出的路径  当前cmakelists下的bin
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

#配置头文件的搜索路径，项目中所有头文件的搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/db)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)
include_directories(${PROJECT_SOURCE_DIR}/include/server/model)
include_directories(${PROJECT_SOURCE_DIR}/include/server/redis)
# 加载子目录的cmakelists
add_subdirectory(src)
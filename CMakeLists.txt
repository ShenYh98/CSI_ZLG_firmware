cmake_minimum_required(VERSION 3.0)  # 指定最低版本要求

# 设置项目名称
project(MyProject) 

# 检查是否已经指定了编译器
if(NOT DEFINED CMAKE_C_COMPILER)
  set(CMAKE_C_COMPILER gcc)
endif()

if(NOT DEFINED CMAKE_CXX_COMPILER)
  set(CMAKE_CXX_COMPILER g++)
endif()

# 打印当前编译工具
message("当前编译工具: ${CMAKE_CXX_COMPILER}")

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)  # 可执行文件输出到 bin 目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/plug)  # 生成的库文件输出到 lib 目录

# 添加头文件搜索路径
include_directories(${CMAKE_SOURCE_DIR}/src/inc_base)

# 添加链接库搜索路径
link_directories(${CMAKE_SOURCE_DIR}/src/lib_base)

# 添加源文件
file(GLOB_RECURSE SOURCES 
	${CMAKE_SOURCE_DIR}/src/code/*.cpp
	${CMAKE_SOURCE_DIR}/src/code/*.c
)

# 判断是否输入了参数
if(ARGUMENT)
	# 生成库文件
	add_library(${ARGUMENT} SHARED ${SOURCES})
  
	file(GLOB_RECURSE LIBRARIES 
	  ${CMAKE_SOURCE_DIR}/src/lib_base/*.so
	)
	# 链接库文件
	target_link_libraries(${ARGUMENT} ${LIBRARIES})

	find_package(Boost REQUIRED COMPONENTS system)
	target_link_libraries(${ARGUMENT} PRIVATE Boost::system)
else()
	# 生成可执行文件
	add_executable(executable ${SOURCES} ${CMAKE_SOURCE_DIR}/src/main.cpp)
  
	file(GLOB_RECURSE LIBRARIES 
	  ${CMAKE_SOURCE_DIR}/src/lib_base/*.so
	)
	# 链接库文件
	target_link_libraries(executable ${LIBRARIES})

	find_package(Boost REQUIRED COMPONENTS system)
	target_link_libraries(executable PRIVATE Boost::system)
endif()

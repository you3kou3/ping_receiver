
cmake_minimum_required(VERSION 3.5)

set(TARGET_NAME ping_receiver)
project(${TARGET_NAME})

#set(CU_LIB_INSTALL_LIBS ON)
#set(CU_LIB_INSTALL_INCLUDES ON)
#set (CU_SCHEMAS_INSTALL_CPP ON)
#set (CU_SCHEMAS_INSTALL_ALL ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# BUILD PROJECT TARGET =====
option(CMAKE_USE_WIN32_THREADS_INIT "using WIN32 threads" ON)


set(Boost_USE_STATIC_LIBS ON)
find_package(Boost REQUIRED COMPONENTS log log_setup thread date_time filesystem system program_options locale )
include_directories(${Boost_INCLUDE_DIR})

#add_definitions("-Wall -std=c++1y")
#add_definitions("-DBOOST_LOG_DYN_LINK")

file(GLOB TARGET_SOURCES sources/*(.c|.cpp|.h|.hpp))
add_executable(${TARGET_NAME} ${TARGET_SOURCES})
target_link_libraries(${TARGET_NAME} ${Boost_LIBRARIES})


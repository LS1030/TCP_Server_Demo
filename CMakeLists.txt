cmake_minimum_required(VERSION 3.20)
project(TCP_Server_Demo)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

# include
include_directories(./Inc)

# SRC
aux_source_directory(./Src USER_SRC)

# Set the output folder where your program will be created
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(TCP_Server_Demo ${USER_SRC})

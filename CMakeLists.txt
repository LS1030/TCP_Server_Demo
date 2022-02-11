cmake_minimum_required(VERSION 3.20)
project(TCP_Server_Demo)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

#OpenCV
#find_package(OpenCV REQUIRED)
#include_directories(${OpenCV_INCLUDE_DIRS})

# include
include_directories(./Inc)

# SRC
aux_source_directory(./Src USER_SRC)

# Set the output folder where your program will be created
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

# link_libraries
set(LINK_LIB
#        ${OpenCV_LIBS}
        )

add_executable(TCP_Server_Demo ${USER_SRC})
target_link_libraries(TCP_Server_Demo ${LINK_LIB})

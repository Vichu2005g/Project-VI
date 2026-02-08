cmake_minimum_required(VERSION 3.14)
project(Project-VI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include crow directory
include_directories(${CMAKE_SOURCE_DIR}/third_party/crow/include)

# Add all source files
add_executable(Project-VI 
    src/main.cpp 
    Models/Car.cpp
    src/database/database.cpp
    src/database/sqlite3.c
)

# Include directories for headers
target_include_directories(Project-VI PRIVATE 
    ${CMAKE_SOURCE_DIR}/third_party/crow/include
    ${CMAKE_SOURCE_DIR}/third_party/asio-1.36.0/include
    ${CMAKE_SOURCE_DIR}/Models
    ${CMAKE_SOURCE_DIR}/src/database
    ${CMAKE_SOURCE_DIR}/src/Routes
    ${CMAKE_SOURCE_DIR}/src/utils   
)


# Windows: Link winsock for networking
if(WIN32)
    target_link_libraries(Project-VI PRIVATE ws2_32)
endif()
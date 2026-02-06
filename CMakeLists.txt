cmake_minimum_required(VERSION 3.14)
project(Project-VI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# Fetch Asio 
FetchContent_Declare(
  asio
  GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
  GIT_TAG asio-1-30-2
)
FetchContent_MakeAvailable(asio)

# Configure Crow to use Asio
set(CROW_USE_BOOST OFF CACHE BOOL "" FORCE)
set(ASIO_INCLUDE_DIR "${asio_SOURCE_DIR}/asio/include")

FetchContent_Declare(
  Crow
  GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
  GIT_TAG master
)
FetchContent_MakeAvailable(Crow)

# Find SQLite3
find_package(SQLite3 REQUIRED)

# adding the soruce files
add_executable(Project-VI 
    src/main.cpp 
    Models/Car.cpp
    src/database/database.cpp
)

# Include the directories
target_include_directories(Project-VI PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/Models
    ${CMAKE_CURRENT_SOURCE_DIR}/src/database
)

# Linkign libraries
target_link_libraries(Project-VI PRIVATE 
    Crow::Crow
    SQLite::SQLite3
)
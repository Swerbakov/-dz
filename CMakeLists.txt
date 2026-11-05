cmake_minimum_required(VERSION 3.16)
project(clients_db CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

add_executable(clients_db main.cpp client_manager.cpp)
target_include_directories(clients_db PRIVATE ${PQXX_INCLUDE_DIRS})
target_link_libraries(clients_db PRIVATE ${PQXX_LIBRARIES})

cmake_minimum_required(VERSION 3.21)
project(myapp)
find_package(Boost REQUIRED)
add_executable(app main.cpp)
target_include_directories(app PRIVATE ${Boost_INCLUDE_DIRS})

add_executable(server server.cpp)
target_include_directories(server PRIVATE ${Boost_INCLUDE_DIRS})

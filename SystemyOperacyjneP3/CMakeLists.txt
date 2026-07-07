cmake_minimum_required(VERSION 3.20)
project(SystemyOperacyjne3SystemGlosowania)

set(CMAKE_CXX_STANDARD 17)

add_executable(central central.cpp)
target_link_libraries(central pthread)

add_executable(local local.cpp)
target_link_libraries(local pthread)

add_executable(launcher launcher.cpp)

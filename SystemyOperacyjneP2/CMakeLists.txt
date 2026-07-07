cmake_minimum_required(VERSION 3.30)
project(SystemyOperacyjneP2)

set(CMAKE_CXX_STANDARD 20)

add_executable(SystemyOperacyjneP2 main.cpp
        philosopher.cpp
        philosopher.hpp
        waiter.cpp
        waiter.hpp)

cmake_minimum_required(VERSION 3.10)
project(PidSim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

#Qt widgets
find_package(Qt5 COMPONENTS Widgets REQUIRED)


add_library(pid_lib src/pid.cpp)

target_include_directories(pid_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Unit testing with Google Test
find_package(GTest REQUIRED)
enable_testing()

#tests for pid
add_executable(pid_tests tests/test_pid.cpp)
target_link_libraries(pid_tests PRIVATE pid_lib GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(pid_tests)


# Main application Qt GUI
add_executable(pidsim
    src/main.cpp
    src/pid.h
    src/pid.cpp
    src/PIDUI.h
    src/PIDUI.cpp
    )

target_link_libraries(pidsim PRIVATE pid_lib Qt5::Widgets)

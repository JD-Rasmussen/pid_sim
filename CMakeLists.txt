cmake_minimum_required(VERSION 3.10)
project(PidSim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(pidsim
    src/main.cpp
    src/pid.h
    src/pid.cpp
    src/PIDUI.h
    src/PIDUI.cpp
    src/testpid.cpp
    )

target_link_libraries(pidsim PRIVATE Qt5::Widgets)

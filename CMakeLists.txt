cmake_minimum_required(VERSION 3.10)
project(PidSim LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

add_executable(PidSim
    src/main.cpp
    src/pid_panel.cpp
    src/pid_panel.h
)

target_link_libraries(PidSim PRIVATE Qt5::Widgets)

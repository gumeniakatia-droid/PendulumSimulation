cmake_minimum_required(VERSION 3.20)
 
project(PendulumSimulation LANGUAGES CXX)
if (MSVC)
    add_compile_options(/utf-8)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML COMPONENTS Graphics Window System CONFIG REQUIRED)

add_executable(PendulumSimulation main.cpp)

target_link_libraries(PendulumSimulation PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
)
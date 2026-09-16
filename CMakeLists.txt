
cmake_minimum_required(VERSION 3.16)

project(PianoSimulator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SFML 2.5 COMPONENTS graphics window system audio REQUIRED)

add_executable(PianoSimulator
    src/main.cpp
)

target_link_libraries(PianoSimulator
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)


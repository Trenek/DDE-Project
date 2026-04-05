cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")

project(Project VERSION 1.0 LANGUAGES CXX)

add_subdirectory(dependencies)

add_executable(Project
    "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/common/draw.cpp"
)
target_link_libraries(Project ${LINK_LIBRARIES})
target_include_directories(Project PUBLIC 
    ${INCLUDE_DIRECTORIES}
    "${CMAKE_CURRENT_SOURCE_DIR}/common"
)

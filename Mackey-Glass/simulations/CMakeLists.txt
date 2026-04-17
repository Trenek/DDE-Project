project(Trajectory LANGUAGES CXX)
add_executable(Trajectory
    "${CMAKE_CURRENT_SOURCE_DIR}/trajectory.cpp"
)
target_link_libraries(Trajectory Common)

project(Bifurcation LANGUAGES CXX)
add_executable(Bifurcation
    "${CMAKE_CURRENT_SOURCE_DIR}/bifurcation.cpp"
)
target_link_libraries(Bifurcation Common)

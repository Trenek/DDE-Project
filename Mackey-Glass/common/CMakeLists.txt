add_library(Common
    "${CMAKE_CURRENT_SOURCE_DIR}/draw.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/mackeyGlass.cpp"
)
target_link_libraries(Common ${LINK_LIBRARIES})
target_include_directories(Common PUBLIC 
    ${INCLUDE_DIRECTORIES}
    "${CMAKE_CURRENT_SOURCE_DIR}/"
)

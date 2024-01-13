cmake_minimum_required(VERSION 3.0.0)

project(venom-renderer)

file(COPY assets DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(ext/assimp)

add_subdirectory(test/loader)
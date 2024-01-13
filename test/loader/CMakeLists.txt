cmake_minimum_required(VERSION 3.0.0)

project("LoaderTest")

INCLUDE_DIRECTORIES(
  ${Assimp_SOURCE_DIR}/include
  ${Assimp_SOURCE_DIR}/code
  ${OPENGL_INCLUDE_DIR}
  ${GLUT_INCLUDE_DIR}
  ${Assimp_SOURCE_DIR}/samples/freeglut/include
)

LINK_DIRECTORIES(
  ${Assimp_BINARY_DIR}
  ${Assimp_BINARY_DIR}/lib
)

add_executable("LoaderTest" loadertest.cpp)

target_link_libraries(${PROJECT_NAME} assimp)

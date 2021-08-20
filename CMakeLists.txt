cmake_minimum_required(VERSION 3.12)

project(BluePlanet)

add_executable(BluePlanet main.cpp)

target_include_directories(BluePlanet PRIVATE deps/glm
                                               deps/stb
                                               deps/glfw/include
                                               deps/glew/include)

target_link_directories(BluePlanet PRIVATE deps/glfw/lib-vc2019
                                           deps/glew/lib/Release/x64)

target_link_libraries(BluePlanet PRIVATE glfw3.lib
                                         glew32.lib
                                         opengl32.lib)

add_custom_command(TARGET BluePlanet POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/deps/glew/bin/Release/x64/glew32.dll" "${CMAKE_BINARY_DIR}/glew32.dll")

add_executable(Vectors Vectors.cpp)

target_include_directories(Vectors PRIVATE deps/glm)


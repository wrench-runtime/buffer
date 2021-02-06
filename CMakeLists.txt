cmake_minimum_required(VERSION 3.6)

# The project name
project(wren_buffer)

# The sources shared between the main program and the tests
add_library(wren_buffer_wrench SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/buffer.c)
target_include_directories(wren_buffer_wrench PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_definitions(wren_buffer_wrench PUBLIC WRENCH_PLUGIN)
set_target_properties(wren_buffer_wrench PROPERTIES PREFIX "")
set_target_properties(wren_buffer_wrench PROPERTIES OUTPUT_NAME "buffer")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/wren/buffer.wren ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/buffer.wren COPYONLY)
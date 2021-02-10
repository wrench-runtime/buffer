cmake_minimum_required(VERSION 3.6)

# The project name
project(module_buffer)

# The sources shared between the main program and the tests
add_library(module_buffer SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/buffer.c)
#target_include_directories(module_buffer_wrench PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
#target_compile_definitions(module_buffer_wrench PUBLIC WRENCH_PLUGIN)
set_target_properties(module_buffer PROPERTIES PREFIX "")
set_target_properties(module_buffer PROPERTIES OUTPUT_NAME "buffer")
target_link_libraries(module_buffer PUBLIC wren_runtime)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/wren/buffer.wren ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/buffer.wren COPYONLY)

# The sources shared between the main program and the tests
add_library(module_buffer_static STATIC ${CMAKE_CURRENT_SOURCE_DIR}/src/buffer.c)
target_link_libraries(module_buffer_static PUBLIC wren_runtime)


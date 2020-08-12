cmake_minimum_required(VERSION 3.15)
project(tests LANGUAGES CXX)

include(utils)
file(GLOB_RECURSE sources "*.cpp" "*.h")
add_executable(cherry_engine_test ${sources})
enable_warnings(cherry_engine_test)
target_link_libraries(cherry_engine_test PRIVATE CherryEngine)
add_test(NAME cherry_engine_test COMMAND cherry_engine_test)

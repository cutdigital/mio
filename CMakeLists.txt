cmake_minimum_required(VERSION 3.1...3.27)

project(
  mio-project
  VERSION 1.0
  LANGUAGES C)

#
# Require out-of-source builds
#
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

add_library(mio STATIC 
  ${CMAKE_CURRENT_SOURCE_DIR}/source/mio.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/obj.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/off.c
  ${CMAKE_CURRENT_SOURCE_DIR}/source/stl.c)

target_include_directories(mio PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(PROJECT_IS_TOP_LEVEL)
    add_executable(example ${CMAKE_CURRENT_SOURCE_DIR}/example/main.c)

    target_include_directories(example PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(example PRIVATE mio)
    target_compile_definitions(example PRIVATE -DDATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/example/data")

    if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND UNIX)
      message(STATUS "enable valgrind debug flags")
      target_compile_options(example PRIVATE -ggdb3 )
    endif()

endif()
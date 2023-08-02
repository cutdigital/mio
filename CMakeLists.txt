cmake_minimum_required(VERSION 3.1...3.27)

project(
  mio-project
  VERSION 1.0
  LANGUAGES C)

### Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

add_library(mio STATIC ${CMAKE_CURRENT_SOURCE_DIR}/source/mio.c)
target_include_directories(mio PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(PROJECT_IS_TOP_LEVEL)
    add_executable(mio-test-app ${CMAKE_CURRENT_SOURCE_DIR}/app/main.c)

    target_include_directories(mio-test-app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    target_link_libraries(mio-test-app PRIVATE mio)
    target_compile_definitions(mio-test-app PRIVATE -DDATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/app/data")
endif()
cmake_minimum_required(VERSION 3.15...3.27)

project(mio-tests VERSION 1.0 LANGUAGES C CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# ============================================================================
# Find or fetch GoogleTest
# ============================================================================

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)

# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# ============================================================================
# Build the MIO library
# ============================================================================

add_library(mio STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../source/mio.c
  ${CMAKE_CURRENT_SOURCE_DIR}/../source/obj.c
  ${CMAKE_CURRENT_SOURCE_DIR}/../source/off.c
  ${CMAKE_CURRENT_SOURCE_DIR}/../source/stl.c)

target_include_directories(mio
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../source)

# Compiler warnings
target_compile_options(mio PRIVATE
  $<$<C_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra>
  $<$<C_COMPILER_ID:MSVC>:/W4>)

# ============================================================================
# Build the test executable
# ============================================================================

add_executable(mio_tests
  test_mio.cpp)

target_link_libraries(mio_tests
  PRIVATE
    mio
    GTest::gtest
    GTest::gtest_main)

# Include test directory for test utilities
target_include_directories(mio_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR})

# Compiler warnings for tests
target_compile_options(mio_tests PRIVATE
  $<$<CXX_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra>
  $<$<CXX_COMPILER_ID:MSVC>:/W4>)

# ============================================================================
# Register tests with CTest
# ============================================================================

include(GoogleTest)
gtest_discover_tests(mio_tests
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  PROPERTIES
    LABELS "unit"
)

# ============================================================================
# Custom test targets
# ============================================================================

# Add custom target to run tests with verbose output
add_custom_target(run_tests
  COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --verbose
  DEPENDS mio_tests
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Running all tests with verbose output")

# Add custom target to run tests with colored output
add_custom_target(run_tests_color
  COMMAND $<TARGET_FILE:mio_tests> --gtest_color=yes
  DEPENDS mio_tests
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
  COMMENT "Running tests with colored output")

# ============================================================================
# Test coverage (optional, requires gcov/lcov)
# ============================================================================

option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)

if(ENABLE_COVERAGE AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(mio PRIVATE --coverage)
  target_link_options(mio PRIVATE --coverage)
  
  target_compile_options(mio_tests PRIVATE --coverage)
  target_link_options(mio_tests PRIVATE --coverage)
  
  # Add coverage target
  find_program(LCOV lcov)
  find_program(GENHTML genhtml)
  
  if(LCOV AND GENHTML)
    add_custom_target(coverage
      COMMAND ${LCOV} --directory . --zerocounters
      COMMAND $<TARGET_FILE:mio_tests>
      COMMAND ${LCOV} --directory . --capture --output-file coverage.info
      COMMAND ${LCOV} --remove coverage.info '/usr/*' '*/googletest/*' --output-file coverage.info
      COMMAND ${GENHTML} coverage.info --output-directory coverage_report
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      COMMENT "Generating code coverage report"
      DEPENDS mio_tests)
  endif()
endif()

# ============================================================================
# Print configuration summary
# ============================================================================

message(STATUS "")
message(STATUS "MIO Test Configuration:")
message(STATUS "  C Compiler: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message(STATUS "  C++ Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "  C Standard: ${CMAKE_C_STANDARD}")
message(STATUS "  C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "  Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "  Coverage Enabled: ${ENABLE_COVERAGE}")
message(STATUS "")

cmake_minimum_required(VERSION 3.15...3.27)

project(
  mio-project
  VERSION 1.0
  DESCRIPTION "Mesh I/O library for multiple formats"
  LANGUAGES C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

# Build options
option(MIO_BUILD_EXAMPLES "Build example programs" ${PROJECT_IS_TOP_LEVEL})
option(MIO_BUILD_SHARED "Build shared library instead of static" OFF)

# Library type selection
if(MIO_BUILD_SHARED)
    set(LIB_TYPE SHARED)
else()
    set(LIB_TYPE STATIC)
endif()

# Create library
add_library(mio ${LIB_TYPE}
  source/mio.c
  source/obj.c
  source/off.c
  source/ply.c
  source/stl.c
  source/third-party/plyfile.c)

# Set library properties
set_target_properties(mio PROPERTIES
  VERSION ${PROJECT_VERSION}
  SOVERSION ${PROJECT_VERSION_MAJOR}
  PUBLIC_HEADER "include/mio.h")

# Target include directories
target_include_directories(mio
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/source)

# Compiler warnings
target_compile_options(mio PRIVATE
  $<$<C_COMPILER_ID:GNU,Clang,AppleClang>:-Wall -Wextra -Wpedantic>
  $<$<C_COMPILER_ID:MSVC>:/W4>)

# Position independent code for static libraries
if(NOT MIO_BUILD_SHARED)
    set_target_properties(mio PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# Add alias for consistent naming when used as subproject
add_library(mio::mio ALIAS mio)

# Example executable
if(MIO_BUILD_EXAMPLES)
    add_executable(mio_example example/main.c)
    
    target_link_libraries(mio_example PRIVATE mio::mio)
    
    target_compile_definitions(mio_example PRIVATE
      DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/example/data")
    
    set_target_properties(mio_example PROPERTIES
      OUTPUT_NAME example)
endif()

# Installation rules
if(PROJECT_IS_TOP_LEVEL)
    include(GNUInstallDirs)
    
    install(TARGETS mio
      EXPORT mioTargets
      LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
      ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
      RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
      PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
    
    install(EXPORT mioTargets
      FILE mioTargets.cmake
      NAMESPACE mio::
      DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mio)
    
    # Generate package config files
    include(CMakePackageConfigHelpers)
    
    write_basic_package_version_file(
      "${CMAKE_CURRENT_BINARY_DIR}/mioConfigVersion.cmake"
      VERSION ${PROJECT_VERSION}
      COMPATIBILITY SameMajorVersion)
    
    configure_package_config_file(
      "${CMAKE_CURRENT_SOURCE_DIR}/cmake/mioConfig.cmake.in"
      "${CMAKE_CURRENT_BINARY_DIR}/mioConfig.cmake"
      INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mio)
    
    install(FILES
      "${CMAKE_CURRENT_BINARY_DIR}/mioConfig.cmake"
      "${CMAKE_CURRENT_BINARY_DIR}/mioConfigVersion.cmake"
      DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/mio)
endif()

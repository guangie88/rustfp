cmake_minimum_required(VERSION 3.3)

# project variables
project(rustfp VERSION 0.1.0 LANGUAGES CXX)

# general fixed compiler settings
if(${MSVC})
  set(DEBUG_FLAGS /W4)
else()
  set(DEBUG_FLAGS -Wall)
endif()

add_compile_options("$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
include(CMakeToolsHelpers OPTIONAL)

add_subdirectory(deps/optional-lite)
add_subdirectory(deps/variant)

# rustfp
add_library(rustfp INTERFACE)

# since optional-lite does not have a robust CMake mechanism
# rustfp subsumes all the include operations of it
target_include_directories(rustfp
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/optional-lite/include>
    $<INSTALL_INTERFACE:include>)

target_link_libraries(rustfp
  INTERFACE
    mpark_variant)

install(TARGETS rustfp EXPORT rustfp)

install(DIRECTORY include/rustfp DESTINATION include)
install(DIRECTORY deps/optional-lite/include/nonstd DESTINATION include)

install(EXPORT rustfp
  FILE rustfp-targets.cmake
  NAMESPACE rustfp::
  DESTINATION lib/cmake/rustfp)

include(CMakePackageConfigHelpers)

configure_package_config_file(
  cmake/rustfp-config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/rustfp-config.cmake"
  INSTALL_DESTINATION lib/cmake/rustfp
  NO_CHECK_REQUIRED_COMPONENTS_MACRO)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/rustfp-config-version.cmake"
  VERSION ${rustfp_VERSION}
  COMPATIBILITY SameMajorVersion)

install(
  FILES
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/rustfp-config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake/rustfp-config-version.cmake"
  DESTINATION lib/cmake/rustfp)

# rustfp_unit_test
if(RUSTFP_INCLUDE_UNIT_TESTS)
  add_executable(rustfp_unit_test
    src/rustfp_unit_test/unit_test_main.cpp
    src/rustfp_unit_test/unit_test.cpp)

  enable_testing()
  add_test(rustfp_unit_test rustfp_unit_test)

  set_property(TARGET rustfp_unit_test PROPERTY CXX_STANDARD 14)

  # Catch2 does not have modern practice for CMake
  target_include_directories(rustfp_unit_test
    PRIVATE
      $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/Catch2/single_include>)

  target_link_libraries(rustfp_unit_test
    PRIVATE
      rustfp)

  if(RUSTFP_INCLUDE_TEST_COVERAGE)
    target_compile_options(rustfp_unit_test
      PRIVATE
        -coverage)

    target_link_libraries(rustfp_unit_test
      PRIVATE
        gcov)
  endif()
endif()

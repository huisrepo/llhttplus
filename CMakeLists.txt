project(llhttp++)
cmake_minimum_required (VERSION 3.14)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
    llhttp 
    URL "https://github.com/nodejs/llhttp/archive/refs/tags/release/v6.0.6.zip"
)

FetchContent_MakeAvailable(llhttp)

add_library(
    llhttp++
    STATIC
    src/llhttp++.cpp
)

target_link_libraries(
    llhttp++
    PRIVATE
    llhttp
)

target_include_directories(
    llhttp++
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
	$<INSTALL_INTERFACE:${MODULE_ARGS_INCLUDE_DIRS}>
)

# ---------------------------------------------------------------------------------------
# Test
# ---------------------------------------------------------------------------------------
option(ENABLE_TEST "Should enable test" OFF)
if(ENABLE_TEST)
    add_subdirectory(test)
endif()

# ---------------------------------------------------------------------------------------
# Install
# ---------------------------------------------------------------------------------------
include(CMakePackageConfigHelpers)

set(PROJECT_NAMESPACE llhttp++)
set(LIB_INSTALL_DIR lib/)
set(config_targets_file "llhttp++-config-targets.cmake")

configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/llhttp++-config.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/llhttp++-config.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}/share/llhttp++
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/llhttp++-config-version.cmake
  VERSION 0.0.1
  COMPATIBILITY SameMajorVersion
)

install(
	DIRECTORY	include
	DESTINATION ${CMAKE_INSTALL_PREFIX}
)

install(
	TARGETS	llhttp++
	EXPORT  llhttp++
)

install(
	EXPORT		llhttp++
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/syrup
	NAMESPACE   ${PROJECT_NAMESPACE}:: 
	FILE		${config_targets_file}
)

install(
	FILES ${CMAKE_CURRENT_BINARY_DIR}/llhttp++-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/llhttp++-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/llhttp++
)
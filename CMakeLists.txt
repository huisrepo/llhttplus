project(llhttplus)
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
    llhttplus
    STATIC
    src/llhttplus.cpp
)

target_link_libraries(
    llhttplus
    PRIVATE
    llhttp
)

target_include_directories(
    llhttplus
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

set(PROJECT_NAMESPACE llhttplus)
set(LIB_INSTALL_DIR lib/)
set(config_targets_file "llhttplus-config-targets.cmake")

configure_package_config_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/llhttplus-config.cmake.in
	${CMAKE_CURRENT_BINARY_DIR}/llhttplus-config.cmake
	INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}/share/llhttplus
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/llhttplus-config-version.cmake
  VERSION 0.0.1
  COMPATIBILITY SameMajorVersion
)

install(
	DIRECTORY	include
	DESTINATION ${CMAKE_INSTALL_PREFIX}
)

install(
	TARGETS	llhttplus
	EXPORT  llhttplus
)

install(
	EXPORT		llhttplus
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/llhttplus
	NAMESPACE   ${PROJECT_NAMESPACE}:: 
	FILE		${config_targets_file}
)

install(
	FILES ${CMAKE_CURRENT_BINARY_DIR}/llhttplus-config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/llhttplus-config-version.cmake
    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/llhttplus
)
cmake_minimum_required(VERSION 2.6)
project(libswarm_logger)
cmake_policy(SET CMP0007 NEW)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

SET(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -Wnon-virtual-dtor -fprofile-arcs -ftest-coverage -fPIC -O0")

SET(MAJOR_VERSION 0)
SET(MINOR_VERSION 1)
SET(PATCH_VERSION 1)
SET(SO_VERSION    0)

#possible modules that can be enabled/disabled
SET(COMPILE_EXAMPLES YES CACHE BOOL "Compile example programs")

#set the include directory
include_directories(${CMAKE_SOURCE_DIR}/include)

#set the source directory
add_subdirectory(src)
add_subdirectory(include)

#the examples
IF(COMPILE_EXAMPLES)
    add_subdirectory(src/example)
ENDIF()


#packaging stuff
INCLUDE(InstallRequiredSystemLibraries)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "libswarm_logger")
SET(CPACK_PACKAGE_VENDOR "eZuce Inc. <info@ezuce.com>")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
INCLUDE(CPack)



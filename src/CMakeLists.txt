
set(Boost_NO_BOOST_CMAKE ON)

find_package(Boost COMPONENTS filesystem system thread REQUIRED)
find_package(Poco REQUIRED)

MESSAGE(STATUS "Found Poco: ${Poco_LIBRARIES}")
MESSAGE(STATUS "Found Boost: ${Boost_LIBRARIES}")

# configure a header file to pass some of the CMake settings
# to the source code
#configure_file("${PROJECT_SOURCE_DIR}/src/version.h.in" "${PROJECT_BINARY_DIR}/gen/version.h")

#
# Common
#
file(GLOB swarm_common_lib_sources common/*.c*)
add_library(swarm_common SHARED ${swarm_common_lib_sources})
add_library(swarm_common_static STATIC ${swarm_common_lib_sources})
target_link_libraries(swarm_common ${Boost_LIBRARIES} ${Poco_LIBRARIES})
target_link_libraries(swarm_common_static ${Boost_LIBRARIES} ${Poco_LIBRARIES})
set_target_properties(swarm_common PROPERTIES OUTPUT_NAME swarm_common)
set_target_properties(swarm_common_static PROPERTIES OUTPUT_NAME swarm_common)
set(VERSION_STRING ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION})
set_target_properties(swarm_common swarm_common_static PROPERTIES VERSION "${VERSION_STRING}" SOVERSION "${SO_VERSION}")
install(TARGETS swarm_common swarm_common_static
                      LIBRARY DESTINATION lib
                      ARCHIVE DESTINATION lib
                      RUNTIME DESTINATION bin)

#
# Logger
#
file(GLOB swarm_logger_lib_sources logger/*.c*)
add_library(swarm_logger SHARED ${swarm_logger_lib_sources})
add_library(swarm_logger_static STATIC ${swarm_logger_lib_sources})
target_link_libraries(swarm_logger swarm_common)
target_link_libraries(swarm_logger_static swarm_common_static)
set_target_properties(swarm_logger PROPERTIES OUTPUT_NAME swarm_logger)
set_target_properties(swarm_logger_static PROPERTIES OUTPUT_NAME swarm_logger)
set(VERSION_STRING ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION})
set_target_properties(swarm_logger swarm_logger_static PROPERTIES VERSION "${VERSION_STRING}" SOVERSION "${SO_VERSION}")
install(TARGETS swarm_logger swarm_logger_static
                      LIBRARY DESTINATION lib
                      ARCHIVE DESTINATION lib
                      RUNTIME DESTINATION bin)

#
# application
#
file(GLOB swarm_application_lib_sources application/*.c*)
add_library(swarm_application SHARED ${swarm_application_lib_sources})
add_library(swarm_application_static STATIC ${swarm_application_lib_sources})
target_link_libraries(swarm_application swarm_common)
target_link_libraries(swarm_application_static swarm_common_static)
set_target_properties(swarm_application PROPERTIES OUTPUT_NAME swarm_application)
set_target_properties(swarm_application_static PROPERTIES OUTPUT_NAME swarm_application)
set(VERSION_STRING ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION})
set_target_properties(swarm_application swarm_application_static PROPERTIES VERSION "${VERSION_STRING}" SOVERSION "${SO_VERSION}")
install(TARGETS swarm_application swarm_application_static
                      LIBRARY DESTINATION lib
                      ARCHIVE DESTINATION lib
                      RUNTIME DESTINATION bin)
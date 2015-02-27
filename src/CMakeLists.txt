
set(Boost_NO_BOOST_CMAKE ON)

find_package(Boost COMPONENTS filesystem system thread REQUIRED)

find_package(Poco COMPONENTS Foundation REQUIRED)

file(GLOB swarm_logger_lib_sources *.c*)


# configure a header file to pass some of the CMake settings
# to the source code
#configure_file("${PROJECT_SOURCE_DIR}/src/version.h.in" "${PROJECT_BINARY_DIR}/gen/version.h")

message(STATUS "Boost Libraries ----> ${Boost_LIBRARIES}")


add_library(swarm_logger SHARED ${swarm_logger_lib_sources})
add_library(swarm_logger_static STATIC ${swarm_logger_lib_sources})
target_link_libraries(swarm_logger ${Boost_LIBRARIES} ${Poco_LIBRARIES})
target_link_libraries(swarm_logger_static ${Boost_LIBRARIES} ${Poco_LIBRARIES})
set_target_properties(swarm_logger PROPERTIES OUTPUT_NAME swarm_logger)
set_target_properties(swarm_logger_static PROPERTIES OUTPUT_NAME swarm_logger)

set(VERSION_STRING ${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION})
set_target_properties(swarm_logger swarm_logger_static PROPERTIES VERSION "${VERSION_STRING}" SOVERSION "${SO_VERSION}")


install(TARGETS swarm_logger swarm_logger_static
                      LIBRARY DESTINATION lib
                      ARCHIVE DESTINATION lib
                      RUNTIME DESTINATION bin)

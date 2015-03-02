file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/gen)

#
# Logger
#
add_executable(swarm_logger_example logger.cpp)
target_link_libraries(swarm_logger_example swarm_logger)

#
# Application
#
add_executable(swarm_application_example application.cpp)
target_link_libraries(swarm_application_example swarm_logger swarm_application)
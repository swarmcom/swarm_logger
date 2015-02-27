file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/gen)
add_executable(swarm_logger_example main.cpp)
target_link_libraries(swarm_logger_example swarm_logger)

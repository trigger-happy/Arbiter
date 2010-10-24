include_directories(${Boost_INCLUDE_DIRS} ${APR_INCLUDE_DIRS})

set(RUNNER_SRC runner.cpp)
set(RUNNER_HDR)

add_executable(runner ${RUNNER_SRC})

target_link_libraries(runner ${Boost_system_LIBRARY} ${Boost_regex_LIBRARY} ${Boost_program_options_LIBRARY} ${APR_LIBRARIES})


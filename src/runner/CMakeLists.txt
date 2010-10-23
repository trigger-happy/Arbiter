include_directories(${Boost_INCLUDE_DIRS} ${APR_INCLUDE_DIRS} ${LIB_PATH})

set(RUNNER_SRC runner.cpp Executor.cpp ProblemSetCache.cpp)
set(RUNNER_HDR Executor.h ProblemSetCache.h)

add_executable(runner ${RUNNER_SRC})

target_link_libraries(runner polarssl ${Boost_system_LIBRARY} ${Boost_regex_LIBRARY} ${Boost_program_options_LIBRARY} ${APR_LIBRARIES})

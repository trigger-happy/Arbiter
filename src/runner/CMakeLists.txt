include_directories(${Boost_INCLUDE_DIRS} ${APR_INCLUDE_DIRS} ${LIB_PATH})

set(RUNNER_SRC runner.cpp Executor.cpp ProblemSetCache.cpp OptionsHandler.cpp)
set(RUNNER_HDR Executor.h ProblemSetCache.h OptionsHandler.h)

add_executable(runner ${RUNNER_SRC})

target_link_libraries(runner polarssl ${Boost_SYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${APR_LIBRARIES})

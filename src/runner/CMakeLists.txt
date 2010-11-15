include_directories(${Boost_INCLUDE_DIRS} ${APR_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} ${LIB_PATH} ${COMMON_DIR})

set(RUNNER_SRC runner.cpp Executor.cpp ProblemSetCache.cpp OptionsParser.cpp)
set(RUNNER_HDR Executor.h ProblemSetCache.h OptionsParser.h)

add_executable(runner ${RUNNER_SRC} "${COMMON_DIR}/EasyUnzip.cpp")

target_link_libraries(runner polarssl minizip ${ZLIB_LIBRARIES} ${Boost_SYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY} ${Boost_PROGRAM_OPTIONS_LIBRARY} ${Boost_DATE_TIME_LIBRARY} ${Boost_THREAD_LIBRARY} ${APR_LIBRARIES})

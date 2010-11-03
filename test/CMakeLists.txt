add_executable(dbo_test dbo_test.cpp)
target_link_libraries(dbo_test ${Wt_LIBRARIES})

add_executable(db_test db_test.cpp
						../src/server/sqlitedb.cpp)
target_link_libraries(db_test ${Wt_LIBRARIES}
						${Boost_unit_test_framework_LIBRARY})

include_directories(${LIB_PATH} ${APR_INCLUDE_DIRS})
add_executable(unzip_test unzip_test.cpp)
target_link_libraries(unzip_test minizip ${APR_LIBRARIES})

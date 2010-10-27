add_executable(dbo_test dbo_test.cpp)
target_link_libraries(dbo_test ${Wt_LIBRARIES})

add_executable(tin_test tin_test.cpp)
target_link_libraries(tin_test ${Boost_LIBRARIES} ${Wt_LIBRARIES})
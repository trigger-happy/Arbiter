add_executable(dbo_test dbo_test.cpp)
target_link_libraries(dbo_test ${Wt_LIBRARIES})

add_executable(server_test server_test.cpp)
target_link_libraries(server_test network)

add_executable(client_test client_test.cpp)
target_link_libraries(client_test network)


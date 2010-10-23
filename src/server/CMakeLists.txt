include_directories(${Boost_INCLUDE_DIRS} ${Wt_INCLUDE_DIR})
add_subdirectory(admin)
add_subdirectory(contestant)
add_subdirectory(scoreboard)
add_subdirectory(judge)

set(SERVER_SRC login.cpp)

add_executable(server ${SERVER_SRC})
link_directories(${Boost_LIBRARY_DIRS})

target_link_libraries(server ${Boost_LIBRARIES} ${Wt_LIBRARIES})
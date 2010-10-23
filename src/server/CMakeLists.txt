include_directories(${Boost_INCLUDE_DIRS} ${Wt_INCLUDE_DIR})
add_subdirectory(admin)
add_subdirectory(contestant)
add_subdirectory(scoreboard)
add_subdirectory(judge)

set(LOGIN_SRC login.cpp)
set(CONTESTANT_SRC contestant.cpp)

add_executable(login ${LOGIN_SRC})
add_executable(contestant ${CONTESTANT_SRC})

link_directories(${Boost_LIBRARY_DIRS})

target_link_libraries(login ${Boost_LIBRARIES} ${Wt_LIBRARIES})
target_link_libraries(contestant ${Boost_LIBRARIES} ${Wt_LIBRARIES})
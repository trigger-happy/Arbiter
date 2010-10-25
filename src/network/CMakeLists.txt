include_directories(${Boost_INCLUDE_DIRS} ${APR_INCLUDE_DIRS} ${LIB_PATH})

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS Message.proto)
set(NETWORK_SRC ${PROTO_SRCS} RunnerConnection.cpp ServerNetwork.cpp RunnerNetwork.cpp)
set(NETWORK_HDR ${PROTO_HDRS} Connection.h RunnerConnection.h ServerNetwork.h RunnerNetwork.h)

add_library(network ${NETWORK_SRC})
add_executable(testclient testclient.cpp)
add_executable(testserver testserver.cpp)

target_link_libraries(network polarssl pthread ${PROTOBUF_LIBRARY} ${Boost_system_LIBRARY} ${Boost_regex_LIBRARY} ${Boost_date_time_LIBRARY} ${Boost_program_options_LIBRARY} ${APR_LIBRARIES})
target_link_libraries(testclient boost_system network)
target_link_libraries(testserver boost_system network)
include_directories(${Boost_INCLUDE_DIRS} ${APR_INCLUDE_DIRS} ${LIB_PATH})

PROTOBUF_GENERATE_CPP(PROTO_SRCS PROTO_HDRS Message.proto)
set(NETWORK_SRC ${PROTO_SRCS} RunnerConnection.cpp ServerNetwork.cpp RunnerNetwork.cpp)
set(NETWORK_HDR ${PROTO_HDRS} Connection.h RunnerConnection.h ServerNetwork.h RunnerNetwork.h)

add_library(network ${NETWORK_SRC})

target_link_libraries(network polarssl pthread ${PROTOBUF_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${Boost_REGEX_LIBRARY} ${Boost_DATE_TIME_LIBRARY})

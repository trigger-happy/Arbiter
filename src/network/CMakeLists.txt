include_directories(${Boost_INCLUDE_DIRS} ${APR_INCLUDE_DIRS} ${LIB_PATH})

set(NETWORK_SRC Message.pb.cc RunnerConnection.cpp ServerNetwork.cpp RunnerNetwork.cpp)
set(NETWORK_HDR Message.pb.h Connection.h RunnerConnection.h ServerNetwork.h RunnerNetwork.h)

add_library(network ${NETWORK_SRC})

target_link_libraries(network polarssl pthread protobuf ${Boost_system_LIBRARY} ${Boost_regex_LIBRARY} ${Boost_program_options_LIBRARY} ${APR_LIBRARIES})

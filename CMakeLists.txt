cmake_minimum_required(VERSION 3.18)
project(server)
set(CMAKE_CXX_STANDARD 17)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREAD ON)
find_package(Boost 1.74.0 REQUIRED COMPONENTS system thread regex log_setup log)
IF(Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
    LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
ENDIF(Boost_FOUND)

set(USED_LIBS ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_REGEX_LIBRARY})

#set(CMAKE_CXX_FLAGS "-L /usr/lib/ -lboost_system -lboost_thread -lpthread")

add_executable(
        rm
        src/main/main.cpp
        src/shm/shm.cpp
        src/api/rm_api.h
        src/api/rm_api.cpp
        src/api/rm_api_private.h
        src/api/rm_api_private.cpp
        src/udp/udp_sender.cpp
        src/udp/udp_receiver.cpp
        src/udp/udp_payload.hpp
        src/udp/udp_buffer.hpp
        src/util/timer.cpp
)

add_executable(
        rm_test_loop
        src/test/test_loop.cpp
        src/shm/shm.cpp
        src/api/rm_api.h
        src/api/rm_api.cpp
        src/api/rm_api_private.h
        src/api/rm_api_private.cpp
        src/udp/udp_sender.cpp
        src/udp/udp_receiver.cpp
        src/udp/udp_payload.hpp
        src/udp/udp_buffer.hpp
        src/util/timer.cpp
)

add_executable(
        rm_test_api
        src/test/test_api.cpp
        src/shm/shm.cpp
        src/api/rm_api.h
        src/api/rm_api.cpp
        src/api/rm_api_private.h
        src/api/rm_api_private.cpp
        src/udp/udp_sender.cpp
        src/udp/udp_receiver.cpp
        src/udp/udp_payload.hpp
        src/udp/udp_buffer.hpp
        src/util/timer.cpp
)

add_executable(
        rm_test_big_packets
        src/test/test_big_packets.cpp
        src/shm/shm.cpp
        src/api/rm_api.h
        src/api/rm_api.cpp
        src/api/rm_api_private.h
        src/api/rm_api_private.cpp
        src/udp/udp_sender.cpp
        src/udp/udp_receiver.cpp
        src/udp/udp_payload.hpp
        src/udp/udp_buffer.hpp
        src/util/timer.cpp
)

add_executable(
        application_simulator
        src/application_simulator/application_simulator.cpp
        src/application_simulator/application_simulator.hpp
        src/api/rm_api.h
)

add_executable(
        application_simulator_big
        src/application_simulator/application_simulator_big.cpp
        src/application_simulator/application_simulator_big.hpp
        src/api/rm_api.h
)

target_link_libraries(rm  ${USED_LIBS} librt.so)
target_link_libraries(rm pthread Boost::log Boost::log_setup )

target_link_libraries(rm_test_loop  ${USED_LIBS} librt.so)
target_link_libraries(rm_test_loop pthread Boost::log Boost::log_setup )

target_link_libraries(rm_test_api  ${USED_LIBS} librt.so)
target_link_libraries(rm_test_api pthread Boost::log Boost::log_setup )

target_link_libraries(rm_test_big_packets  ${USED_LIBS} librt.so)
target_link_libraries(rm_test_big_packets pthread Boost::log Boost::log_setup )

target_link_libraries(application_simulator  ${USED_LIBS} librt.so)
target_link_libraries(application_simulator pthread Boost::log Boost::log_setup )

target_link_libraries(application_simulator_big  ${USED_LIBS} librt.so)
target_link_libraries(application_simulator_big pthread Boost::log Boost::log_setup )
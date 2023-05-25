cmake_minimum_required(VERSION 3.18)
project(server)
set(CMAKE_CXX_STANDARD 17)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREAD ON)
find_package(Boost 1.42.0 REQUIRED COMPONENTS system thread regex)
IF(Boost_FOUND)
    INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
    LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
ENDIF(Boost_FOUND)

set(USED_LIBS ${Boost_SYSTEM_LIBRARY} ${Boost_THREAD_LIBRARY} ${Boost_REGEX_LIBRARY})

#set(CMAKE_CXX_FLAGS "-L /usr/lib/ -lboost_system -lboost_thread -lpthread")

add_executable(
        rm_poc
        main.cpp
        shm_api.hpp
        shm_api.cpp
        shm.cpp
        udp_sender.cpp
        udp_receiver.cpp
        timer.cpp
)

add_executable(
        application_simulator
        application_simulator.cpp
        application_simulator.hpp
)
target_link_libraries(rm_poc  ${USED_LIBS} librt.so)

